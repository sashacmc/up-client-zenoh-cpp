// SPDX-FileCopyrightText: 2024 Contributors to the Eclipse Foundation
//
// See the NOTICE file(s) distributed with this work for additional
// information regarding copyright ownership.
//
// This program and the accompanying materials are made available under the
// terms of the Apache License Version 2.0 which is available at
// https://www.apache.org/licenses/LICENSE-2.0
//
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>

#include <up-cpp/communication/RpcClient.h>
#include <up-cpp/communication/RpcServer.h>
#include <up-cpp/datamodel/builder/Payload.h>
#include <up-cpp/datamodel/builder/Uuid.h>
#include <up-cpp/datamodel/serializer/Uuid.h>
// #include <up-cpp/datamodel/validator/UMessage.h>
// #include <up-cpp/datamodel/validator/UUri.h>
// #include <google/protobuf/util/message_differencer.h>

#include <up-transport-zenoh-cpp/ZenohUTransport.h>

#include <iostream>

// using MsgDiff = google::protobuf::util::MessageDifferencer;

using namespace std::chrono_literals;

namespace {

using namespace uprotocol::v1;
using uprotocol::communication::RpcClient;
using uprotocol::communication::RpcServer;

constexpr std::string_view ZENOH_CONFIG_FILE = BUILD_REALPATH_ZENOH_CONF;

struct MyUUri {
	std::string auth = "";
	uint32_t ue_id = 0x8000;
	uint32_t ue_version_major = 1;
	uint32_t resource_id = 1;

	operator uprotocol::v1::UUri() const
	{
		UUri ret;
		ret.set_authority_name(auth);
		ret.set_ue_id(ue_id);
		ret.set_ue_version_major(ue_version_major);
		ret.set_resource_id(resource_id);
		return ret;
	}

	std::string to_string() const
	{
		return std::string("<< ") + UUri(*this).ShortDebugString() + " >>";
	}
};

class RpcClientServerTest : public testing::Test {
protected:
	MyUUri rpc_service_uuri_{"me_authority", 65538, 1, 32600};
	MyUUri client_ident_{"def_client_auth", 65538, 1, 0};
	MyUUri server_ident_{"def_server_auth", 65538, 1, 0};
	using Transport = uprotocol::transport::ZenohUTransport;
	std::shared_ptr<Transport> client_transport_;
	std::shared_ptr<Transport> server_transport_;

	// Run once per TEST_F.
	// Used to set up clean environments per test.
	void SetUp() override
	{
		client_transport_ = std::make_shared<Transport>(client_ident_, ZENOH_CONFIG_FILE);
		EXPECT_NE(nullptr, client_transport_);
		server_transport_ = std::make_shared<Transport>(server_ident_, ZENOH_CONFIG_FILE);
		EXPECT_NE(nullptr, server_transport_);
	}

	void TearDown() override
	{
		client_transport_ = nullptr;
		server_transport_ = nullptr;		
	}

	// Run once per execution of the test application.
	// Used for setup of all tests. Has access to this instance.
	RpcClientServerTest() = default;
	~RpcClientServerTest() = default;

	// Run once per execution of the test application.
	// Used only for global setup outside of tests.
	static void SetUpTestSuite() {}
	static void TearDownTestSuite() {}
};

uprotocol::datamodel::builder::Payload fakePayload() {
	using namespace uprotocol::datamodel;

	auto uuid = builder::UuidBuilder::getBuilder();
	auto uuid_str = serializer::uuid::AsString::serialize(uuid.build());

	return builder::Payload(
	    std::move(uuid_str),
	    UPayloadFormat::UPAYLOAD_FORMAT_TEXT);
}

// TODO replace
TEST_F(RpcClientServerTest, SomeTestName)
{
	using namespace std;

	UMessage server_capture;

	auto serverOrStatus = RpcServer::create(
		server_transport_,
		rpc_service_uuri_,
		[this, &server_capture](const UMessage& message) {
			cout << "in server callback" << endl;
			UPayloadFormat format = UPayloadFormat::UPAYLOAD_FORMAT_TEXT;
			std::string responseData = "RPC Response";
			uprotocol::datamodel::builder::Payload payload(responseData, format);
			return payload;
		},
		UPayloadFormat::UPAYLOAD_FORMAT_TEXT);
	ASSERT_TRUE(serverOrStatus.has_value());
	ASSERT_NE(serverOrStatus.value(), nullptr);

	auto payload = fakePayload();
	auto payload_content = payload.buildCopy();

	auto client = RpcClient(client_transport_, rpc_service_uuri_, UPriority::UPRIORITY_CS4, 1000ms);

	bool callback_called = false;
	uprotocol::v1::UMessage received_response;
	uprotocol::communication::RpcClient::InvokeHandle client_handle;
	EXPECT_NO_THROW(client_handle = client.invokeMethod(
		std::move(payload),
		[this, &callback_called, &received_response](auto maybe_response) {
			cout << "in client callback " << typeid(maybe_response).name() << endl;
			if (maybe_response.has_value()) {
				cout << maybe_response.value().ShortDebugString() << endl;
			}
			else {
				cout << "does not have value " << maybe_response.error().ShortDebugString() << endl;
			}
		    callback_called = true;
		    // EXPECT_TRUE(maybe_response);
		    // received_response = std::move(maybe_response).value();
		}));
}

}  // namespace
