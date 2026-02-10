#pragma once

#include <drogon/orm/CoroMapper.h>   // nebo cesta podle tvého projektu
#include <models/model_interface/IOrmModel.h>
#include <string>
#include <vector>

#include <drogon/orm/Mapper.h>
#include <drogon/utils/coroutine.h>

namespace drogon::orm
{

	template <typename T>
	class JoinCoroMapper : public CoroMapper<T>
	{
		using SingleRowCallback = typename CoroMapper<T>::SingleRowCallback;
		static_assert(std::is_base_of_v<model_interface::IOrmModel, T>,
			"T must derive from ");
	public:
		using CoroMapper<T>::CoroMapper; // zdìdí konstruktor (DbClientPtr)

		/// Nastaví base FROM tabulku + alias (defaultnì T::tableName + "t")
		JoinCoroMapper& from(std::string table = T::tableName, std::string alias = "t")
		{
			baseTable_ = std::move(table);
			baseAlias_ = std::move(alias);
			return *this;
		}

		/// Pøidá JOIN (øetìzí se)
		JoinCoroMapper& join(std::string table, std::string alias, std::string onExpr)
		{
			joins_.push_back(" JOIN " + table + " " + alias + " ON " + onExpr);
			return *this;
		}

		JoinCoroMapper& leftJoin(std::string table, std::string alias, std::string onExpr)
		{
			joins_.push_back(" LEFT JOIN " + table + " " + alias + " ON " + onExpr);
			return *this;
		}

		/// Vyèistí JOIN konfiguraci (užiteèné pro reuse stejného mapperu)
		JoinCoroMapper& clearJoins()
		{
			joins_.clear();
			baseTable_.clear();
			baseAlias_.clear();
			return *this;
		}



		inline internal::MapperAwaiter<T> findByPrimaryKeyJoin(
			const typename T::PrimaryKeyType& key)
		{
			if constexpr (!std::is_same_v<typename T::PrimaryKeyType, void>)
			{
				auto lb = [this, key](SingleRowCallback&& callback,
					ExceptPtrCallback&& errCallback) mutable {

						static_assert(
							internal::has_sqlForFindingByPrimaryKey<T>::value,
							"Model must implement sqlForFindingByPrimaryKeyJoin()");

						std::string sql = T::sqlForFindingByPrimaryKey();

						if (this->forUpdate_)
						{
							sql += " for update";
						}

						this->clear();

						auto binder = *(this->client_) << std::move(sql);

						this->outputPrimaryKeyToBinder(key, binder);

						binder >> [callback = std::move(callback),
							errCallback](const Result& r) {

							if (r.size() == 0)
							{
								errCallback(std::make_exception_ptr(
									UnexpectedRows("0 rows found")));
							}
							else if (r.size() > 1)
							{
								errCallback(std::make_exception_ptr(
									UnexpectedRows("More than one row")));
							}
							else
							{
								callback(T(r[0]));
							}
							};

						binder >> std::move(errCallback);
						binder.exec();
					};

				return internal::MapperAwaiter<T>(std::move(lb));
			}
			else
			{
				LOG_FATAL << "No primary key";
				abort();
			}
		}

		/// Najde více øádkù jako T(row) pøes SELECT <baseAlias>.* + JOINy + Criteria
		inline internal::MapperAwaiter<std::vector<T>> findByJoin(const Criteria& criteria = Criteria())
		{
			auto lb = [this, criteria](typename Mapper<T>::MultipleRowsCallback&& callback,
				typename CoroMapper<T>::ExceptPtrCallback&& errCallback)
				{
					// 1) FROM
					const std::string table = baseTable_.empty() ? std::string(T::tableName) : baseTable_;
					const std::string alias = baseAlias_.empty() ? std::string("t") : baseAlias_;

					std::string sql = "select " + alias + ".* from ";
					sql += table + " " + alias;

					// 2) JOINy
					for (auto& j : joins_)
						sql += j;

					// 3) WHERE + paging + order
					bool hasParameters = false;
					if (criteria)
					{
						hasParameters = true;
						sql += " where ";
						sql += criteria.criteriaString();
					}

					sql.append(this->orderByString_);

					if (this->limit_ > 0)
					{
						hasParameters = true;
						sql.append(" limit $?");
					}
					if (this->offset_ > 0)
					{
						hasParameters = true;
						sql.append(" offset $?");
					}

					if (hasParameters)
						sql = this->replaceSqlPlaceHolder(sql, "$?");

					if (this->forUpdate_)
						sql += " for update";

					auto binder = *(this->client_) << std::move(sql);

					if (criteria)
						criteria.outputArgs(binder);

					if (this->limit_ > 0)
						binder << this->limit_;
					if (this->offset_ > 0)
						binder << this->offset_;

					this->clear(); // vyèistí limit/offset/orderBy/forUpdate stejnì jako CoroMapper

					binder >> [callback = std::move(callback)](const Result& r)
						{
							std::vector<T> ret;
							ret.reserve(r.size());
							for (auto const& row : r)
								ret.emplace_back(T(row));
							callback(ret);
						};

					binder >> std::move(errCallback);
				};

			return internal::MapperAwaiter<std::vector<T>>(std::move(lb));
		}

		inline internal::MapperAwaiter<size_t> deleteByJoin(const Criteria& criteria)
		{
			auto lb = [this, criteria](typename Mapper<T>::CountCallback&& callback,
				typename CoroMapper<T>::ExceptPtrCallback&& errCallback)
				{
					const std::string table = baseTable_.empty() ? std::string(T::tableName) : baseTable_;
					const std::string alias = baseAlias_.empty() ? std::string("t") : baseAlias_;

					std::string sql = "DELETE FROM " + alias + " USING ";
					sql += table + " " + alias;

					for (auto& j : joins_)
						sql += j;

					if (criteria)
					{
						sql += " WHERE ";
						sql += criteria.criteriaString();
						sql = this->replaceSqlPlaceHolder(sql, "$?");
					}

					auto binder = *(this->client_) << std::move(sql);

					if (criteria)
						criteria.outputArgs(binder);

					this->clear();

					binder >> [callback = std::move(callback)](const Result& r)
						{
							callback(r.affectedRows());
						};

					binder >> std::move(errCallback);
				};

			return internal::MapperAwaiter<size_t>(std::move(lb));
		}

		inline internal::MapperAwaiter<size_t> deleteOneJoin(const Criteria& criteria)
		{
			return deleteByJoin(criteria && Criteria(baseAlias_ + ".id", CompareOperator::IsNotNull));
		}

		template<typename... Arguments>
		internal::MapperAwaiter<size_t> updateByJoin(
			const std::vector<std::string>& colNames,
			const Criteria& criteria,
			Arguments&&... args)
		{
			auto lb = [this,
				colNames,
				criteria,
				... args = std::forward<Arguments>(args)]
				(typename Mapper<T>::CountCallback&& callback,
					typename CoroMapper<T>::ExceptPtrCallback&& errCallback) mutable
				{
					const std::string table = baseTable_.empty() ? std::string(T::tableName) : baseTable_;
					const std::string alias = baseAlias_.empty() ? std::string("t") : baseAlias_;

					std::string sql = "UPDATE " + table + " " + alias + " SET ";

					for (const auto& col : colNames)
					{
						sql += col + " = $?,";
					}
					sql.back() = ' ';

					sql += " FROM ";

					bool first = true;
					for (auto& j : joins_)
					{
						if (first)
						{
							sql += j.substr(6); // odstraní " JOIN "
							first = false;
						}
						else
							sql += j;
					}

					if (criteria)
					{
						sql += " WHERE ";
						sql += criteria.criteriaString();
					}

					sql = this->replaceSqlPlaceHolder(sql, "$?");

					auto binder = *(this->client_) << std::move(sql);

					(void)std::initializer_list<int>{(binder << args, 0)...};

					if (criteria)
						criteria.outputArgs(binder);

					this->clear();

					binder >> [callback = std::move(callback)](const Result& r)
						{
							callback(r.affectedRows());
						};

					binder >> std::move(errCallback);
				};

			return internal::MapperAwaiter<size_t>(std::move(lb));
		}

		inline internal::MapperAwaiter<bool> existsJoin(const Criteria& criteria)
		{
			auto lb = [this, criteria](std::function<void(bool)>&& callback,
				typename CoroMapper<T>::ExceptPtrCallback&& errCallback)
				{
					const std::string table = baseTable_.empty() ? std::string(T::tableName) : baseTable_;
					const std::string alias = baseAlias_.empty() ? std::string("t") : baseAlias_;

					std::string sql = "SELECT 1 FROM ";
					sql += table + " " + alias;

					for (auto& j : joins_)
						sql += j;

					if (criteria)
					{
						sql += " WHERE ";
						sql += criteria.criteriaString();
						sql = this->replaceSqlPlaceHolder(sql, "$?");
					}

					sql += " LIMIT 1";

					auto binder = *(this->client_) << std::move(sql);

					if (criteria)
						criteria.outputArgs(binder);

					this->clear();

					binder >> [callback = std::move(callback)](const Result& r)
						{
							callback(!r.empty());
						};

					binder >> std::move(errCallback);
				};

			return internal::MapperAwaiter<bool>(std::move(lb));
		}



		/// Jednoznaèný øádek pøes JOINy
		inline internal::MapperAwaiter<T> findOneJoin(const Criteria& criteria)
		{
			auto lb = [this, criteria](typename Mapper<T>::SingleRowCallback&& callback,
				typename CoroMapper<T>::ExceptPtrCallback&& errCallback)
				{
					const std::string table = baseTable_.empty() ? std::string(T::tableName) : baseTable_;
					const std::string alias = baseAlias_.empty() ? std::string("t") : baseAlias_;

					std::string sql = "select " + alias + ".* from ";
					sql += table + " " + alias;

					for (auto& j : joins_)
						sql += j;

					bool hasParameters = false;
					if (criteria)
					{
						hasParameters = true;
						sql += " where ";
						sql += criteria.criteriaString();
					}

					sql.append(this->orderByString_);

					// findOne: nastavíme limit 1, ale necháme i uživatelský limit (pokud ho chceš)
					hasParameters = true;
					sql.append(" limit $?");

					if (this->offset_ > 0)
					{
						hasParameters = true;
						sql.append(" offset $?");
					}

					if (hasParameters)
						sql = this->replaceSqlPlaceHolder(sql, "$?");

					if (this->forUpdate_)
						sql += " for update";

					auto binder = *(this->client_) << std::move(sql);

					if (criteria)
						criteria.outputArgs(binder);

					binder << (this->limit_ > 0 ? this->limit_ : 1UL);

					if (this->offset_ > 0)
						binder << this->offset_;

					this->clear();

					binder >> [errCallback, callback = std::move(callback)](const Result& r)
						{
							if (r.size() == 0)
								errCallback(std::make_exception_ptr(UnexpectedRows("0 rows found")));
							else if (r.size() > 1)
								errCallback(std::make_exception_ptr(UnexpectedRows("Found more than one row")));
							else
								callback(T(r[0]));
						};

					binder >> std::move(errCallback);
				};

			return internal::MapperAwaiter<T>(std::move(lb));
		}

		/// COUNT(*) pøes JOINy (typicky pro stránkování)
		inline internal::MapperAwaiter<size_t> countJoin(const Criteria& criteria = Criteria())
		{
			auto lb = [this, criteria](typename Mapper<T>::CountCallback&& callback,
				typename CoroMapper<T>::ExceptPtrCallback&& errCallback)
				{
					const std::string table = baseTable_.empty() ? std::string(T::tableName) : baseTable_;
					const std::string alias = baseAlias_.empty() ? std::string("t") : baseAlias_;

					std::string sql = "select count(*) from ";
					sql += table + " " + alias;

					for (auto& j : joins_)
						sql += j;

					if (criteria)
					{
						sql += " where ";
						sql += criteria.criteriaString();
						sql = this->replaceSqlPlaceHolder(sql, "$?");
					}

					auto binder = *(this->client_) << std::move(sql);

					if (criteria)
						criteria.outputArgs(binder);

					this->clear();

					binder >> [callback = std::move(callback)](const Result& r)
						{
							callback(r[0][(Row::SizeType)0].as<size_t>());
						};

					binder >> std::move(errCallback);
				};

			return internal::MapperAwaiter<size_t>(std::move(lb));
		}

		JoinCoroMapper<T>& limit(size_t limit)
		{
			CoroMapper<T>::limit(limit);
			return *this;
		}

		JoinCoroMapper<T>& offset(size_t offset)
		{
			CoroMapper<T>::offset(offset);
			return *this;
		}

		JoinCoroMapper<T>& orderBy(const std::string& colName,
			const SortOrder& order = SortOrder::ASC)
		{
			CoroMapper<T>::orderBy(colName, order);
			return *this;
		}

		JoinCoroMapper<T>& orderBy(size_t colIndex,
			const SortOrder& order = SortOrder::ASC)
		{
			CoroMapper<T>::orderBy(colIndex, order);
			return *this;
		}

		JoinCoroMapper<T>& paginate(size_t page, size_t perPage)
		{
			CoroMapper<T>::paginate(page, perPage);
			return *this;
		}

		JoinCoroMapper<T>& forUpdate()
		{
			CoroMapper<T>::forUpdate();
			return *this;
		}


	protected:
		std::string baseTable_;
		std::string baseAlias_;
		std::vector<std::string> joins_;
	};

} // namespace drogon::orm
