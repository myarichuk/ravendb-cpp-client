#include "pch.h"
#include "ConnectionDetailsHolder.h"
#include <fstream>
#include <string>

namespace ravendb::client::tests::infrastructure
{
	ConnectionDetailsHolder::~ConnectionDetailsHolder() = default;

	ConnectionDetailsHolder::ConnectionDetailsHolder(const std::string& def_file_name, bool has_certificate = true)
	{ 
		//open definitions file
		std::ifstream def_file(def_file_name);
		if (!def_file)
		{
			throw std::runtime_error(std::string("Can't open ") + def_file_name);
		}
		if (!std::getline(def_file, url))
		{
			throw std::runtime_error(std::string("Can't read url from ") + def_file_name);
		}

		if (!has_certificate)
		{
			return;
		}
		//get certificate
		std::string cert_file_name;
		while (!def_file.eof() && cert_file_name.empty())
		{			
			std::getline(def_file, cert_file_name);
		}

		if(cert_file_name.empty())
			throw std::runtime_error(std::string("Can't read certificate file name from ") + def_file_name);

		std::ifstream cert_file(cert_file_name);
		if (!cert_file)
		{
			throw std::runtime_error(std::string("Can't open ") + cert_file_name);
		}
		{
			std::stringstream stream;
			stream << cert_file.rdbuf();
			cert_details.certificate = stream.str();
		}
		//get key
		std::string key_file_name;
		if (!std::getline(def_file, key_file_name) || key_file_name.empty())
		{
			throw std::runtime_error(std::string("Can't read key file name from ") + def_file_name);
		}
		std::ifstream key_file(key_file_name);
		if (!key_file)
		{
			throw std::runtime_error(std::string("Can't open ") + key_file_name);
		}
		{
			std::stringstream stream;
			stream << key_file.rdbuf();
			cert_details.key = stream.str();
		}
		//get CA file path(optional)
		std::string CA_path_file_name;
		if (!std::getline(def_file, CA_path_file_name))
		{
			return;//defaults will be used
		}
		//no problem if CA_path_file_name.empty() == true
		cert_details.ca_path = CA_path_file_name;

		//get password(optional)
		std::string key_password;
		if (!std::getline(def_file, key_password))
		{
			return;//no password
		}
		//no problem if  key_password.empty() == true
		cert_details.key_password = key_password;
	}

	const impl::CertificateDetails& ConnectionDetailsHolder::get_certificate_details() const
	{
		return cert_details;
	}

	const std::string& ConnectionDetailsHolder::get_url() const
	{
		return url;
	}
}
