// Tryouts.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>

#include "User.h"

#include "GetDocumentsCommand.h"
#include "PutDocumentCommand.h"
#include "RequestExecutor.h"
#include "GetDatabaseRecordOperation.h"
#include "CreateDatabaseOperation.h"

#include "GetDatabaseNamesOperation.h"
#include "DeleteDatabasesOperation.h"
#include "DeleteDocumentCommand.h"
#include "CreateSampleDataOperation.h"
#include "PutIndexesOperation.h"
#include "GetIndexOperation.h"
#include "GetIndexesOperation.h"
#include "QueryCommand.h"
#include "DateTimeOffset.h"
#include "IndexHasChangedOperation.h"
#include "DeleteIndexOperation.h"
#include "DisableIndexOperation.h"
#include "EnableIndexOperation.h"
#include "ResetIndexOperation.h"

namespace
{
	//using fiddler + verbose
	void set_for_fiddler(CURL* curl)
	{
		curl_easy_setopt(curl, CURLOPT_PROXY, "127.0.0.1:8888");
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
	}
}

int main()
{
	using namespace ravendb::client;

	//User user{ "Alexander","Timoshenko" };
	//nlohmann::json j = user;
	//std::string id{ "newID123" };
	//auto _executor = ravendb::client::http::RequestExecutor::create({ "http://127.0.0.1:8080" }, "TEST__DocumentCommandsTests", {},
	//	{ debug_curl_init, nullptr });
	//{
	//	documents::commands::PutDocumentCommand cmd(id, {}, j);
	//	_executor->execute(cmd);
	//}
	//{
	//	documents::commands::DeleteDocumentCommand cmd(id,"dssdf");
	//	_executor->execute(cmd);
	//}

	//auto exec = http::RequestExecutor::create({ "http://127.0.0.1:8080" }, {}, {}, set_for_fiddler);
	//serverwide::DatabaseRecord rec;
	//rec.database_name = "Test";
	//rec.topology.members.push_back("A");
	//serverwide::operations::CreateDatabaseCommand cmd(rec,1);
	//auto res1 = exec->execute(cmd);

	//for (auto& it : res1.topology.promotables_status)
	//{
	//	std::cout << static_cast<uint8_t>(it.second)<<" ";
	//}

	//auto test_suite_executor = http::RequestExecutor::create({ "http://127.0.0.1:8080" }, "Test", {}, set_for_fiddler);
	//tests::infrastructure::CreateSampleDataCommand cmd2;
	//test_suite_executor->execute(cmd2);


	//auto test_suite_executor = http::RequestExecutor::create({ "http://127.0.0.1:8080" }, "Test", {}, set_for_fiddler);
	//std::string id1(500,'a'), id2 = id1+"aaa", id3 = id2+"AAA";

	//documents::commands::GetDocumentsCommand cmd({id1,id2,id3}, {}, true);
	//test_suite_executor->execute(cmd);

	//auto test_suite_executor = http::RequestExecutor::create({ "http://127.0.0.1:8080" }, {}, {}, set_for_fiddler);
	//serverwide::operations::GetDatabaseRecordCommand cmd2("test");

	//auto res2 = exec->execute(cmd2);

	auto test_suite_executor = http::RequestExecutor::create({ "http://127.0.0.1:8080" }, "Northwind", {}, set_for_fiddler);

	//documents::commands::GetDocumentsCommand cmd(
	//	std::vector<std::string>{ "employees/1-A", "orders/830-A" }, { "Freight","ReportsTo"}, false);
	//documents::commands::GetDocumentsCommand cmd(0, 20);
	//std::vector<std::string> ids;
	//for(auto i=1;i<=100;++i)
	//{
	//	ids.push_back("employees/" + std::to_string(i) + "-A");
	//}
	//documents::commands::GetDocumentsCommand cmd(ids, {},false);
	//documents::commands::GetDocumentsCommand cmd("employees/", "", "1-A*", "", 0, 16, false);
	//documents::commands::GetDocumentsCommand cmd(
	//		std::vector<std::string>{ "employees/1-A", "employees/3-A", "employees/3-A", "orders/830-A"}, {"Employee","ReportsTo"}, false);
	//test_suite_executor->execute(cmd);

	documents::indexes::IndexDefinition index{};
	index.name = "MyIndex";
	index.maps = 
	{ 
		R"(
		from order in docs.Orders 
			select new 
			{ 
				order.Employee, 
				order.Company
			})"
	};
	//auto cmd = documents::operations::indexes::PutIndexesOperation({index}).get_command({});
	//auto res = test_suite_executor->execute(*cmd);
	//std::cout << res[0].index_name << std::endl;

	//std::getchar();

	//auto cmd2 = documents::operations::indexes::GetIndexOperation("Orders/ByCompany").get_command({});
	//auto res2 = test_suite_executor->execute(*cmd2);
	//std::cout << res2.name << std::endl;

	//std::getchar();

	//auto cmd3 = documents::operations::indexes::GetIndexesOperation(0,10).get_command({});
	//auto res3 = test_suite_executor->execute(*cmd3);
	//std::cout << res3[1].name << std::endl;

	//std::getchar();

	//auto cmd4 = documents::operations::indexes::IndexHasChangedOperation(index).get_command({});
	//auto res4 = test_suite_executor->execute(*cmd4);
	//std::cout << std::boolalpha << res4 << std::endl;

	//std::getchar();

	//auto cmd5 = documents::operations::indexes::DeleteIndexOperation("MyIndex").get_command({});
	//auto res5 = test_suite_executor->execute(*cmd5);

	//serverwide::operations::DeleteDatabasesOperation op6("something", true, {}, std::chrono::hours(30));
	//auto cmd6 = op6.get_command({});
	//auto res6 = test_suite_executor->execute(*cmd6);

	//auto cmd7 = documents::operations::indexes::EnableIndexOperation("People/Search").get_command({});
	//auto res4 = test_suite_executor->execute(*cmd7);

	auto cmd8 = documents::operations::indexes::ResetIndexOperation("People/Search").get_command({});
	auto res8 = test_suite_executor->execute(*cmd8);

	//std::string query = R"(

	//from Employees
	//where StartsWith(FirstName,$prefix)

	//)";

	//documents::queries::IndexQuery indexQuery(query);
	//indexQuery.query_parameters.emplace("prefix", "A");
	//indexQuery.explain_scores = true;
	//indexQuery.show_timings = true;
	//indexQuery.wait_for_non_stale_results = true;
	//indexQuery.wait_for_non_stale_results_timeout = std::chrono::seconds(5);


	//auto cmd = documents::commands::QueryCommand({}, indexQuery, false , false);
	//auto res = test_suite_executor->execute(cmd);
	//for (auto& result : res.results)
	//{
	//	std::cout << result.at("FirstName").get<std::string>() << " " << result.at("LastName").get<std::string>() << " " <<
	//		result.at("Birthday").get<impl::DateTimeOffset>() << "\n";
	//}
	//std::cout << "last query time " << res.last_query_time << "\n";
	//nlohmann::json j = res.last_query_time;
	//std::cout << "last query time " << j << "\n";

	//documents::queries::QueryResult r = res.create_snapshot();

	//std::getchar();



}

