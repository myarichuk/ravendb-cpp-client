
#include "pch.h"
#include <curl/curl.h>
#include "DocumentStore.h"
#include "DocumentSession.h"
#include "User.h"

namespace
{
	//using fiddler + verbose
	void set_for_fiddler(CURL* curl)
	{
		curl_easy_setopt(curl, CURLOPT_PROXY, "127.0.0.1:8888");
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
	}

	void set_for_verbose(CURL* curl)
	{
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
	}
}

int main()
{
	using namespace ravendb::client;

	auto store = documents::DocumentStore::create();
	store->set_urls({ "http://127.0.0.1:8080" });
	store->set_before_perform(set_for_fiddler);
	store->set_database("Test");
	store->initialize();

	{
		auto session = store->open_session();
		auto user = std::make_shared<tests::infrastructure::entities::User>();
		session.store(user);
		session.save_changes();
	}

}