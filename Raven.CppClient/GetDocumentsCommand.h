#pragma once
#include "stdafx.h"
#include "RavenCommand.h"
#include "GetDocumentsResult.h"
#include "utils.h"
#include <unordered_set>
//TODO put in final project
#include "C:\work\xxhash_cpp\xxhash\xxhash.hpp"


namespace ravendb::client::documents::commands
{
	using ravendb::client::http::RavenCommand, ravendb::client::http::ServerNode;

	class GetDocumentsCommand : public RavenCommand<GetDocumentsResult>
	{
	private:
		std::string _id;
		std::unordered_set<std::string> _ids{};
		std::vector<std::string> _includes{};

		bool _metadataOnly = false;

		std::string _start_with;
		std::string _start_after;
		std::string _matches;
		std::string _exclude;
		std::optional<int32_t> _start;
		std::optional<int32_t> _pageSize;

		//using xxhash_cpp from https://github.com/RedSpah/xxhash_cpp
		//TODO consider the endian issue
		template <typename ConstIterator>
		static int64_t calculate_docs_ids_hash(ConstIterator begin, ConstIterator end)
		{
			xxh::hash_state_t<64> hash_stream;
			for(ConstIterator it = begin; it != end; ++it)
			{
				hash_stream.update(*it);
			}

			return hash_stream.digest();
		}

		void prepareRequestWithMultipleIds(std::ostringstream& pathBuilder, CURL* curl) const
		{
			std::size_t totalLen = 0;
			const auto& uniqueIds = _ids;

			std::for_each(uniqueIds.cbegin(), uniqueIds.cend(), [&](const std::string& id) {totalLen += id.length(); });

			bool isGet = totalLen < 1024;
			// if it is too big, we drop to POST (note that means that we can't use the HTTP cache any longer)
			// we are fine with that, requests to load > 1024 items are going to be rare

			if (isGet)// can use GET
			{
				curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);
				for (const auto& id : uniqueIds)
				{
					pathBuilder << "&id=" << ravendb::client::impl::utils::url_escape(curl, id);
				}
			}
			else // ids too big, must use POST
			{
				uint64_t hash = calculate_docs_ids_hash(uniqueIds.cbegin(), uniqueIds.cend());
				pathBuilder << "&loadHash=" << hash;

				curl_easy_setopt(curl, CURLOPT_HTTPPOST, 1);

				auto&& json_str = nlohmann::json({ {"Ids", uniqueIds} }).dump();

				curl_easy_setopt(curl, CURLOPT_COPYPOSTFIELDS, json_str.c_str());
			}
		}

	public:

		~GetDocumentsCommand() override = default;

		GetDocumentsCommand(int32_t start, int32_t pageSize)
			: _start(start)
			, _pageSize(pageSize)
		{}

		GetDocumentsCommand(std::string id, std::vector<std::string> includes, bool _metadataOnly)
			: _id(std::move(id))
			, _includes(std::move(includes))
			, _metadataOnly(_metadataOnly)
		{}

		GetDocumentsCommand(const std::vector<std::string>& ids, std::vector<std::string> includes, bool _metadataOnly)
			: _ids([&]
		{
			if (ids.empty())
				throw std::invalid_argument("Please supply at least one id");
			else return std::unordered_set<std::string>(ids.cbegin(),ids.cend());
		}())
			, _includes(std::move(includes))
			, _metadataOnly(_metadataOnly)
		{}

		GetDocumentsCommand
			( std::string startWith
			, std::string startAfter
			, std::string matches
			, std::string exclude
			, int32_t start
			, int32_t pageSize
			, bool metadataOnly)
			: _metadataOnly(metadataOnly)
			, _start_with([&]
			{
				if (startWith.empty())
					throw std::invalid_argument("startWith cannot be empty");
				else return std::move(startWith);
			}())
			, _start_after(std::move(startAfter))
			, _matches(std::move(matches))
			, _exclude(std::move(exclude))
			, _start(start)
			, _pageSize(pageSize)

		{}

		void create_request(CURL* curl, const ServerNode& node, std::string& url) const override
		{
			std::ostringstream pathBuilder;
			pathBuilder << node.url << "/databases/" << node.database << "/docs?";

			if (_start.has_value())
			{
				pathBuilder << "&start=" << std::to_string(_start.value());
			}

			if (_pageSize.has_value())
			{
				pathBuilder << "&pageSize=" << std::to_string(_pageSize.value());
			}

			if (_metadataOnly)
			{
				pathBuilder << "&metadataOnly=true";
			}

			if (! _start_with.empty())
			{
				pathBuilder << "&startsWith=" << ravendb::client::impl::utils::url_escape(curl, _start_with);

				if (! _matches.empty())
				{
					pathBuilder << "&matches=" << ravendb::client::impl::utils::url_escape(curl, _matches);
				}

				if (! _exclude.empty())
				{
					pathBuilder << "&exclude=" << ravendb::client::impl::utils::url_escape(curl, _exclude);
				}

				if (! _start_after.empty())
				{
					pathBuilder << "&startAfter=" << ravendb::client::impl::utils::url_escape(curl, _start_after);
				}
			}

			for (auto const& include : _includes)
			{
				pathBuilder << "&include=" << include;
			}

			curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);

			if (! _id.empty())
			{
				pathBuilder << "&id=" << ravendb::client::impl::utils::url_escape(curl, _id);
			}
			else if (! _ids.empty())
			{
				prepareRequestWithMultipleIds(pathBuilder, curl);
			}

			url = pathBuilder.str();
		}

		void set_response(CURL* curl, const nlohmann::json& response, bool from_cache) override
		{
				_result = response;
		}

		bool is_read_request() const noexcept override
		{
			return true;
		}
	};
}
