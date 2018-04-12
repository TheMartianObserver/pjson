#include "../pjson.h"

#include <iostream>
#include <string>
#include <vector>

#include "catch.hpp"

TEST_CASE("Parse a JSON string")
{
  char s[] = R"(
    {
      "x": 1
    }
  )";

  pjson::document doc;
  doc.deserialize_in_place(s);

  REQUIRE(doc.find_key("x") == 1);
}

#if 0

TEST_CASE("Testing some type_name stuff out") {
  REQUIRE(fmt::format("{}", type_name<double>()) == "double");
  REQUIRE(fmt::format("{}", type_name<int>()) == "int");
  REQUIRE_THAT(fmt::format("{}", type_name<std::string>()), Catch::Contains("std") && Catch::Contains("string"));
}

TEST_CASE("Testing some JSON stuff") {

  std::string jstr = R"(
{
  "array": [1,2.0,3.0, 4.0],
  "value": 1.2,
  "val1": 24,
  "str": "rsadfa",
  "group1": {
    "gv1": 1
  },
  "heterogenousArray": ["a", 1],
  "stringArray": ["a1", "a2", "b1", "b2"]
}
  )";

  JsonCfg jcfg(jstr.c_str());

  SECTION("Existence tests") {
    REQUIRE(jcfg.ExistsType<int>("/group1/gv1"_json_pointer));
    REQUIRE(jcfg.Exists("/group1/gv1"_json_pointer));
    REQUIRE(!jcfg.ExistsType<int>("/group1/gv2"_json_pointer));

    REQUIRE(jcfg.Exists("value"));
    REQUIRE(!jcfg.Exists("fdsvalue"));
    REQUIRE(!jcfg.Exists("gv1"));

    JsonCfg s = jcfg["group1"];
    REQUIRE(s.Exists("gv1"));

    JsonCfg s1 = jcfg["/group1"_json_pointer];
    REQUIRE(s1.Exists("gv1"));
  }

  SECTION("Getting existing variable but wrong type") {
    REQUIRE_THROWS_AS(jcfg.GetDouble("str", 42), rce::error);
    REQUIRE_THROWS_WITH(jcfg.GetDouble("str", 42), Catch::Contains("wrong type"));


    REQUIRE_THROWS(jcfg.GetDouble("/str"_json_pointer, 42));
    REQUIRE_THROWS_AS(jcfg.GetDouble("/str"_json_pointer, 42), rce::error);
    REQUIRE_THROWS_WITH(jcfg.GetDouble("/str"_json_pointer, 42), Catch::Contains("wrong type"));

    REQUIRE_THROWS_AS(jcfg.GetDouble("array"), rce::error);
    REQUIRE_THROWS_WITH(jcfg.GetDouble("array"),
                        Catch::Contains("number")
                          && Catch::Contains("wrong type")
                          && Catch::Contains("array"));
    REQUIRE_THROWS_AS(jcfg.GetArrayDouble("val1"), rce::error);
    REQUIRE_THROWS_WITH(jcfg.GetArrayDouble("val1"),
                        Catch::Contains("wrong type"));

    REQUIRE_THROWS_AS(jcfg.GetArrayDouble("heterogenousArray"), rce::error);
  }

  SECTION("Get parameters") {
    REQUIRE(jcfg.GetArrayDouble("array") == std::vector<double>({1.0, 2.0, 3.0, 4.0}));
    REQUIRE(jcfg.GetArrayDouble("dasasdasarray", std::vector<double>({1.0, 2.0})) ==
        std::vector<double>({1.0, 2.0}));
    REQUIRE(jcfg.GetInt("/group1/gv1"_json_pointer) == 1);
  }

  SECTION("Get non-existent double with default") {
    REQUIRE(jcfg.GetDouble("valuetastartrawtr", 1.2) == 1.2);
    REQUIRE(jcfg.GetDouble("valuetastartrawtr", 2) == 2.0);

  }

  SECTION("Get non-existent std::string with default") {
    REQUIRE(jcfg.GetString("valuetastartrawtr", "test") == "test");
    REQUIRE_THROWS_AS(jcfg.GetString("heterogenousArray", "test"), rce::error);
  }

  SECTION("Get double") {
    REQUIRE(jcfg.GetDouble("value") == 1.2);
    REQUIRE_THROWS_AS(jcfg.GetDouble("heterogenousArray"), rce::error);
    REQUIRE_THROWS_AS(jcfg.GetDouble("str"), rce::error);
    REQUIRE_THROWS_AS(jcfg.GetDouble("DFjdsjfksd_NOT_VALID"), rce::error);
  }

  SECTION("Get std::string array") {
    REQUIRE(jcfg.GetArrayString("stringArray") == std::vector<std::string>({"a1", "a2", "b1", "b2"}));
    REQUIRE(jcfg.GetArrayString("stringArrayASDSADASDADS", std::vector<std::string>({"d", "e"})) == std::vector<std::string>({"d", "e"}));
    REQUIRE(jcfg.GetArrayString("stringArrayASDSADASDADS", std::vector<std::string>({"d", "e"})) != std::vector<std::string>({"r", "e"}));
    REQUIRE(jcfg.GetArrayString("/stringArray"_json_pointer) == std::vector<std::string>({"a1", "a2", "b1", "b2"}));
  }

  SECTION("Get std::string") {
    REQUIRE(jcfg.GetString("str") == "rsadfa");
    REQUIRE(jcfg.GetString("/str"_json_pointer) == "rsadfa");
    REQUIRE_THROWS_AS(jcfg.GetString("heterogenousArray"), rce::error);
    REQUIRE_THROWS_AS(jcfg.GetString("value"), rce::error);
    REQUIRE_THROWS_AS(jcfg.GetString("DFjdsjfksd_NOT_VALID"), rce::error);
  }

  SECTION("Get non-existent parameters") {
    REQUIRE(jcfg.GetDouble("v2DoesntExist", 42) == 42);

    REQUIRE_THROWS_AS(jcfg.GetDouble("/v2DoesntExist"_json_pointer), rce::error);

    REQUIRE_THROWS_AS(jcfg.GetDouble("v2DoesntExist"), rce::error);
    REQUIRE_THROWS_WITH(jcfg.GetDouble("v2DoesntExist"),
                        Catch::Contains("key 'v2DoesntExist' not found"));

    REQUIRE_THROWS_AS(jcfg["v2DoesntExist"], rce::error);
  }

  SECTION("Check JSON types") {
    json arrayParam = jcfg["array"].json();
    REQUIRE(arrayParam.is_array());
    REQUIRE_FALSE(arrayParam.is_number());
    REQUIRE(arrayParam.size() == 4);
  }

}

#endif
