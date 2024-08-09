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
#include <up-cpp/datamodel/serializer/UUri.h>
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

constexpr std::string_view ENTITY_URI_STR = "//test0/10001/1/0";
constexpr std::string_view TOPIC_URI_STR = "//test0/10001/1/8000";

struct MyUUri {
	std::string auth = "";
	uint32_t ue_id = 0x8000;
	uint32_t ue_version_major = 1;
	uint32_t resource_id = 1;

	operator uprotocol::v1::UUri() const {
		UUri ret;
		ret.set_authority_name(auth);
		ret.set_ue_id(ue_id);
		ret.set_ue_version_major(ue_version_major);
		ret.set_resource_id(resource_id);
		return ret;
	}

	std::string to_string() const {
		return std::string("<< ") + UUri(*this).ShortDebugString() + " >>";
	}
};

class RpcServerOnlyTest : public testing::Test {
protected:
	MyUUri rpc_service_uuri_{"me_authority", 65538, 1, 32600};
	MyUUri client_ident_{"def_client_auth", 65538, 1, 0};
	MyUUri server_ident_{"def_server_auth", 65538, 1, 0};

	// MyUUri rpc_service_uuri_{"test0", 65537, 1, 32600};
	// MyUUri client_ident_{"test0", 65537, 1, 0};
	// MyUUri server_ident_{"def_server_auth", 65538, 1, 0};

	using Transport = uprotocol::transport::ZenohUTransport;
	std::shared_ptr<Transport> client_transport_;
	std::shared_ptr<Transport> server_transport_;

	// Run once per TEST_F.
	// Used to set up clean environments per test.
	void SetUp() override {
		server_transport_ =
		    std::make_shared<Transport>(server_ident_, ZENOH_CONFIG_FILE);
		EXPECT_NE(nullptr, server_transport_);
	}

	void TearDown() override { server_transport_ = nullptr; }

	// Run once per execution of the test application.
	// Used for setup of all tests. Has access to this instance.
	RpcServerOnlyTest() = default;
	~RpcServerOnlyTest() = default;

	// Run once per execution of the test application.
	// Used only for global setup outside of tests.
	static void SetUpTestSuite() {}
	static void TearDownTestSuite() {}
};

uprotocol::datamodel::builder::Payload fakePayload() {
	using namespace uprotocol::datamodel;

	auto uuid = builder::UuidBuilder::getBuilder();
	auto uuid_str = serializer::uuid::AsString::serialize(uuid.build());

	return builder::Payload(std::move(uuid_str),
	                        UPayloadFormat::UPAYLOAD_FORMAT_TEXT);
}

UUri makeUUri(std::string_view serialized) {
	return uprotocol::datamodel::serializer::uri::AsString::deserialize(
	    static_cast<std::string>(serialized));
}

// TODO replace
TEST_F(RpcServerOnlyTest, SomeTestName) {
	using namespace std;

	cout << makeUUri(TOPIC_URI_STR).ShortDebugString() << endl;
	cout << makeUUri(ENTITY_URI_STR).ShortDebugString() << endl;

	UMessage server_capture;

	auto serverOrStatus = RpcServer::create(
	    server_transport_, rpc_service_uuri_,
	    [this, &server_capture](const UMessage& message) {
		    cout << "in server callback" << endl;
		    UPayloadFormat format = UPayloadFormat::UPAYLOAD_FORMAT_TEXT;
		    std::string responseData = "RPC Response";
		    uprotocol::datamodel::builder::Payload payload(responseData,
		                                                   format);
		    return payload;
	    },
	    UPayloadFormat::UPAYLOAD_FORMAT_TEXT);
	ASSERT_TRUE(serverOrStatus.has_value());
	ASSERT_NE(serverOrStatus.value(), nullptr);

	sleep(20);
}

}  // namespace
