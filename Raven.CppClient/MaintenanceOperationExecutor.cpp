#include "stdafx.h"
#include "MaintenanceOperationExecutor.h"
#include "DocumentStore.h"
#include "CompareStringsEqualIgnoreCase.h"

namespace ravendb::client::documents::operations
{
	std::shared_ptr<http::RequestExecutor> MaintenanceOperationExecutor::get_request_executor()
	{
		if(_request_executor)
		{
			return  _request_executor;
		}

		if(_database_name)
		{
			_request_executor = _store.lock()->get_request_executor(*_database_name);
		}

		return _request_executor;
	}

	void MaintenanceOperationExecutor::assert_database_name_set() const
	{
		if(!_database_name)
		{
			throw std::runtime_error("Cannot use maintenance without a database defined,"
				" did you forget to call for_database?");
		}
	}

	MaintenanceOperationExecutor::MaintenanceOperationExecutor(std::shared_ptr<DocumentStore> store,
		std::optional<std::string> database_name)
		: _store(store)
		, _database_name(std::move(database_name))
	{
		if(!store)
		{
			throw std::invalid_argument("'store' should be non-empty");
		}
		if(!_database_name)
		{
			_database_name = store->get_database();
		}
	}

	std::shared_ptr<MaintenanceOperationExecutor> MaintenanceOperationExecutor::create(
		std::shared_ptr<DocumentStore> store, std::optional<std::string> database_name)
	{
		auto object = std::shared_ptr<MaintenanceOperationExecutor>(new MaintenanceOperationExecutor(store, std::move(database_name)));
		object->_weak_this = object;
		return object;
	}

	std::shared_ptr<serverwide::operations::ServerOperationExecutor> MaintenanceOperationExecutor::server()
	{
		if(!_server_operation_executor)
		{
			_server_operation_executor = serverwide::operations::ServerOperationExecutor::create(_store.lock());
		}
		return _server_operation_executor;
	}

	std::shared_ptr<MaintenanceOperationExecutor> MaintenanceOperationExecutor::for_database(
		std::string database_name) const
	{
		if(_database_name.has_value() &&  impl::utils::CompareStringsEqualIgnoreCase()(*_database_name, database_name))
		{
			return _weak_this.lock();
		}

		return create(_store.lock(), std::move(database_name));
	}

	void MaintenanceOperationExecutor::send(IVoidMaintenanceOperation& operation)
	{
		assert_database_name_set();
		auto command = operation.get_command(get_request_executor()->get_conventions());
		get_request_executor()->execute(*command);
	}

	std::unique_ptr<Operation> MaintenanceOperationExecutor::send_async(const IMaintenanceOperation<OperationIdResult>& operation)
	{
		assert_database_name_set();

		auto command = operation.get_command(get_request_executor()->get_conventions());

		get_request_executor()->execute(*command);
		return std::make_unique<Operation>(get_request_executor(), get_request_executor()->get_conventions(), command->get_result()->operation_id);
	}
}
