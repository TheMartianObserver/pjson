#include "../pjson.h"

#include <cmath>
#include <iostream>

#include "catch.hpp"

TEST_CASE("Parse a JSON string")
{
  char s[] = R"(
    {
      "x": 1,
      "y": 2,
      "a": [5.0, 6],
      "infinity": Inf,
      "infinity2": inf
    }
  )";

  pjson::document doc;
  doc.deserialize_in_place(s, strlen(s));

  REQUIRE(doc.is_object());

  REQUIRE(doc.find_key("x") == 0);
  REQUIRE(doc.get_value_at_index(0).is_numeric());
  REQUIRE(doc.get_value_at_index(0).as_int64(-1) == 1);
  REQUIRE(doc.find_key("y") == 1);
  REQUIRE(doc.find_key("a") == 2);

  const auto& v = doc.get_value_at_index(2);
  REQUIRE(v.is_array());

  REQUIRE(std::isinf(doc.get_value_at_index(3).as_double()));
  REQUIRE(std::isinf(doc.get_value_at_index(4).as_double()));
}

TEST_CASE("Parse a JSON array with nan")
{
  char s[] = R"([nan])";

  pjson::document doc;
  REQUIRE(doc.deserialize_in_place(s, strlen(s)));
  REQUIRE(doc.is_array());
  REQUIRE(doc.get_value_at_index(0).is_double());
  REQUIRE(std::isnan(doc.get_value_at_index(0).as_double()));
}

TEST_CASE("Parse a JSON array with NaN")
{
  char s[] = "[NaN]";

  pjson::document doc;
  REQUIRE(doc.deserialize_in_place(s, strlen(s)));
  REQUIRE(doc.is_array());
  REQUIRE(doc.get_value_at_index(0).is_double());
  REQUIRE(std::isnan(doc.get_value_at_index(0).as_double()));

}

TEST_CASE("Parse a JSON array with too short string ('n')")
{
  char s[] = "[N]";

  pjson::document doc;
  REQUIRE_FALSE(doc.deserialize_in_place(s, strlen(s)));
  REQUIRE(doc.get_error_location() == 1);
}

TEST_CASE("Create a JSON doc")
{
  pjson::document doc;
  doc.set_to_object();
  doc.add_key_value("x", 1, doc.get_allocator());
  doc.add_key_value("y", 2.4, doc.get_allocator());

  REQUIRE(doc.find_key("x") == 0);
  REQUIRE(doc.find_key("y") == 1);
  REQUIRE(doc.find_key("a") == -1);


  auto variant = doc.find_value_variant("x");

  REQUIRE(variant != nullptr);

  pjson::char_vec_t charv;
  doc.serialize(charv, false);

  const char* output = charv.data();

  CAPTURE(output);
  int string_comp_result = strcmp(output, "{\"x\":1,\"y\":2.4}");

  REQUIRE(string_comp_result == 0);
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
