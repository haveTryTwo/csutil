// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <set>
#include <sstream>
#include <string>
#include <vector>

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include <google/protobuf/message.h>

#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

#include "base/file_util.h"

#include "proto/pb_manage.h"
#include "proto/pb_to_json.h"
#include "proto/pb_util.h"

#include "test_base/include/test_base.h"

#include "pb_src/model.pb.h"

std::string GetStringFromJson(const rapidjson::Value &value) { /*{{{*/
  rapidjson::StringBuffer buffer;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
  value.Accept(writer);

  return std::string(buffer.GetString(), buffer.GetSize());
} /*}}}*/

TEST(PBToJsonWithOutExtension, Test_Normal_Person) { /*{{{*/
  using namespace base;

  model::Person person;
  person.set_name("zhangsan");
  person.set_age(100);
  person.set_country_name("China");
  person.set_birthday(1);
  person.set_height(1.73);
  person.set_weight(105.413);
  person.set_is_student(false);
  person.set_resv1("good one");
  person.set_resv2(0x1122334455667788);
  person.set_resv3(0xfe11223344556677);

  std::string json;
  Code ret = proto::PBToJsonWithOutExtension(person, &json);
  EXPECT_EQ(kOk, ret);

  std::string expect_json =
      "{\"name\":\"zhangsan\",\"age\":100,\"country_name\":\"China\",\"birthday\":1,\"height\":1."
      "73,\"weight\":105.41300201416016,\"is_student\":false,\"resv1\":\"good "
      "one\",\"resv2\":1234605616436508552,\"resv3\":-139292509886650761}";
  EXPECT_EQ(expect_json, json);

  // fprintf(stderr, "pb_str:\n%s\n", person.DebugString().c_str());
  // fprintf(stderr, "size:%zu, json:       %s\n", json.size(), json.c_str());
  // fprintf(stderr, "size:%zu, expect_json:%s\n", expect_json.size(), expect_json.c_str());
} /*}}}*/

TEST(PBToJsonWithOutExtension, Test_Normal_Person_Enum) { /*{{{*/
  using namespace base;

  model::Person person;
  person.set_name("zhangsan");
  person.set_birthday(1);
  person.set_is_student(false);
  person.set_resv1("good one");
  person.set_resv2(0x1122334455667788);
  person.set_resv3(0xfe11223344556677);
  person.set_health_status(model::HS_MID);

  std::string json;
  Code ret = proto::PBToJsonWithOutExtension(person, &json);
  EXPECT_EQ(kOk, ret);

  std::string expect_json =
      "{\"name\":\"zhangsan\",\"birthday\":1,\"is_student\":false,\"resv1\":\"good "
      "one\",\"resv2\":1234605616436508552,\"resv3\":-139292509886650761,\"health_status\":102}";
  EXPECT_EQ(expect_json, json);

  // fprintf(stderr, "pb_str:\n%s\n", person.DebugString().c_str());
  // fprintf(stderr, "size:%zu, json:       %s\n", json.size(), json.c_str());
  // fprintf(stderr, "size:%zu, expect_json:%s\n", expect_json.size(), expect_json.c_str());
} /*}}}*/

TEST(PBToJsonWithOutExtension, Test_Normal_Person_UpperField) { /*{{{*/
  using namespace base;

  model::Person person;
  person.set_name("zhangsan");
  person.set_birthday(1);
  person.set_is_student(false);
  person.set_resv1("good one");
  person.set_resv2(0x1122334455667788);
  person.set_resv3(0xfe11223344556677);
  person.set_resv4(0xfe11223344556677);  // Note: upper field will be set of RESV4
  person.set_health_status(model::HS_MID);

  std::string json;
  Code ret = proto::PBToJsonWithOutExtension(person, &json);
  EXPECT_EQ(kOk, ret);

  std::string expect_json =
      "{\"name\":\"zhangsan\",\"birthday\":1,\"is_student\":false,\"resv1\":\"good "
      "one\",\"resv2\":1234605616436508552,\"resv3\":-139292509886650761,\"RESV4\":-"
      "139292509886650761,\"health_status\":102}";
  EXPECT_EQ(expect_json, json);

  fprintf(stderr, "pb_str:\n%s\n", person.DebugString().c_str());
  fprintf(stderr, "size:%zu, json:       %s\n", json.size(), json.c_str());
  fprintf(stderr, "size:%zu, expect_json:%s\n", expect_json.size(), expect_json.c_str());
} /*}}}*/

TEST(PBToJsonWithOutExtension, Test_Normal_Person_Repeated_String) { /*{{{*/
  using namespace base;

  model::Person person;
  person.set_name("lisi");
  person.set_birthday(1);
  person.set_is_student(false);
  person.set_resv1("good one");
  person.set_resv2(0x1122334455667788);
  person.set_resv3(0xfe11223344556677);

  person.add_resv11(0x11);
  person.add_resv11(0x22);
  person.add_resv11(0x33);
  person.add_resv11(0x44);

  person.add_friends("Test1");
  person.add_friends("Test2");
  person.add_friends("Test3");
  person.add_friends("Test4");

  person.set_health_status(model::HS_GOOD);

  std::string json;
  Code ret = proto::PBToJsonWithOutExtension(person, &json);
  EXPECT_EQ(kOk, ret);

  std::string expect_json =
      "{\"name\":\"lisi\",\"birthday\":1,\"is_student\":false,\"resv1\":\"good "
      "one\",\"resv2\":1234605616436508552,\"resv3\":-139292509886650761,\"resv11\":[17,34,51,68],"
      "\"friends\":[\"Test1\",\"Test2\",\"Test3\",\"Test4\"],\"health_status\":101}";
  EXPECT_EQ(expect_json, json);

  // fprintf(stderr, "pb_str:\n%s\n", person.DebugString().c_str());
  // fprintf(stderr, "size:%zu, json:       %s\n", json.size(), json.c_str());
  // fprintf(stderr, "size:%zu, expect_json:%s\n", expect_json.size(), expect_json.c_str());
} /*}}}*/

TEST(PBToJsonWithOutExtension, Test_Normal_Person_Repeated_OtherType) { /*{{{*/
  using namespace base;

  model::Person person;
  person.set_name("lisi");
  person.set_birthday(1);
  person.set_is_student(false);
  person.set_resv1("good one");
  person.set_resv2(0x1122334455667788);
  person.set_resv3(0xfe11223344556677);

  person.add_resv11(0x11);
  person.add_resv11(0x22);
  person.add_resv11(0x33);
  person.add_resv11(0x44);

  person.add_resv12(11.11);
  person.add_resv12(22.11);
  person.add_resv12(33.11);
  person.add_resv12(44.11);

  person.add_resv13(false);
  person.add_resv13(true);
  person.add_resv13(true);
  person.add_resv13(false);

  person.add_resv14("adfas23243");
  person.add_resv14("*lasdfnklwe");
  person.add_resv14("热爱祖国");
  person.add_resv14("好好学习，天天向上！！！");

  person.add_resv15(model::HS_GOOD);
  person.add_resv15(model::HS_MID);
  person.add_resv15(model::HS_BAD);
  person.add_resv15(model::HS_GOOD);

  person.set_health_status(model::HS_GOOD);

  std::string json;
  Code ret = proto::PBToJsonWithOutExtension(person, &json);
  EXPECT_EQ(kOk, ret);

  std::string expect_json =
      "{\"name\":\"lisi\",\"birthday\":1,\"is_student\":false,\"resv1\":\"good "
      "one\",\"resv2\":1234605616436508552,\"resv3\":-139292509886650761,\"resv11\":[17,34,51,68],"
      "\"resv12\":[11.11,22.11,33.11,44.11],\"resv13\":[false,true,true,false],\"resv14\":["
      "\"adfas23243\",\"*lasdfnklwe\",\"热爱祖国\",\"好好学习，天天向上！！！\"],\"resv15\":[101,"
      "102,103,101],\"health_status\":101}";
  EXPECT_EQ(expect_json, json);

  // fprintf(stderr, "pb_str:\n%s\n", person.DebugString().c_str());
  // fprintf(stderr, "size:%zu, json:       %s\n", json.size(), json.c_str());
  // fprintf(stderr, "size:%zu, expect_json:%s\n", expect_json.size(), expect_json.c_str());
} /*}}}*/

TEST(PBToJsonWithOutExtension, Test_Normal_Person_Repeated_Object) { /*{{{*/
  using namespace base;

  model::Person person;
  person.set_name("lisi");
  person.set_birthday(1);
  person.set_is_student(false);
  person.set_resv1("good one");
  person.set_resv2(0x1122334455667788);
  person.set_resv3(0xfe11223344556677);

  model::American *american = person.add_american_friends();
  american->set_id("american_1");
  american->set_name("jack_1");
  american->set_age(21);
  model::Addr *addr = american->mutable_addr();
  addr->set_place("New York");
  addr->set_num(1);
  model::Hobby *hobby = american->add_hobbies();
  hobby->set_name("swimming");
  hobby->set_skill_level(3);
  hobby = american->add_hobbies();
  hobby->set_name("chess");
  hobby->set_skill_level(4);
  hobby = american->add_hobbies();
  hobby->set_name("basketball");
  hobby->set_skill_level(4);

  american = person.add_american_friends();
  american->set_id("american_2");
  american->set_name("jack_2");
  american->set_age(22);

  american = person.add_american_friends();
  american->set_id("american_3");
  american->set_name("jack_3");
  american->set_age(23);

  model::English *english = person.add_english_friends();
  english->set_id("english_1");
  english->set_name("rose_1");
  english->set_age(21);

  english = person.add_english_friends();
  english->set_id("english_2");
  english->set_name("rose_2");
  english->set_age(22);

  english = person.add_english_friends();
  english->set_id("english_3");
  english->set_name("rose_3");
  english->set_age(23);

  person.set_health_status(model::HS_GOOD);

  std::string json;
  Code ret = proto::PBToJsonWithOutExtension(person, &json);
  EXPECT_EQ(kOk, ret);

  std::string expect_json =
      "{\"name\":\"lisi\",\"birthday\":1,\"is_student\":false,\"resv1\":\"good "
      "one\",\"resv2\":1234605616436508552,\"resv3\":-139292509886650761,\"american_friends\":[{"
      "\"id\":\"american_1\",\"name\":\"jack_1\",\"age\":21,\"addr\":{\"place\":\"New "
      "York\",\"num\":1},\"hobbies\":[{\"name\":\"swimming\",\"skill_level\":3},{\"name\":"
      "\"chess\",\"skill_level\":4},{\"name\":\"basketball\",\"skill_level\":4}]},{\"id\":"
      "\"american_2\",\"name\":\"jack_2\",\"age\":22},{\"id\":\"american_3\",\"name\":\"jack_3\","
      "\"age\":23}],\"english_friends\":[{\"id\":\"english_1\",\"name\":\"rose_1\",\"age\":21},{"
      "\"id\":\"english_2\",\"name\":\"rose_2\",\"age\":22},{\"id\":\"english_3\",\"name\":\"rose_"
      "3\",\"age\":23}],\"health_status\":101}";
  EXPECT_EQ(expect_json, json);

  // fprintf(stderr, "pb_str:\n%s\n", person.DebugString().c_str());
  // fprintf(stderr, "size:%zu, json:       %s\n", json.size(), json.c_str());
  // fprintf(stderr, "size:%zu, expect_json:%s\n", expect_json.size(), expect_json.c_str());
} /*}}}*/

TEST(PBToJsonWithOutExtension, Test_Normal_Person_Object) { /*{{{*/
  using namespace base;

  model::Person person;
  person.set_name("lisi");
  person.set_birthday(1);
  person.set_is_student(false);
  person.set_resv1("good one");
  person.set_resv2(0x1122334455667788);
  person.set_resv3(0xfe11223344556677);

  model::Addr *addrs = person.mutable_addrs();
  addrs->set_place("广东深圳");
  addrs->set_num(1);

  person.set_health_status(model::HS_GOOD);

  std::string json;
  Code ret = proto::PBToJsonWithOutExtension(person, &json);
  EXPECT_EQ(kOk, ret);
  std::string expect_json =
      "{\"name\":\"lisi\",\"birthday\":1,\"is_student\":false,\"resv1\":\"good "
      "one\",\"resv2\":1234605616436508552,\"resv3\":-139292509886650761,\"addrs\":{\"place\":"
      "\"广东深圳\",\"num\":1},\"health_status\":101}";
  EXPECT_EQ(expect_json, json);

  // fprintf(stderr, "pb_str:\n%s\n", person.DebugString().c_str());
  // fprintf(stderr, "size:%zu, json:       %s\n", json.size(), json.c_str());
  // fprintf(stderr, "size:%zu, expect_json:%s\n", expect_json.size(), expect_json.c_str());
} /*}}}*/

TEST(PBToJsonWithOutExtension, Test_Normal_Person_Extension) { /*{{{*/
  using namespace base;

  model::Person person;
  person.set_name("lisi");
  person.set_birthday(1);
  person.set_is_student(false);
  person.set_resv1("good one");
  person.set_resv2(0x1122334455667788);
  person.set_resv3(0xfe11223344556677);

  model::Addr *addrs = person.mutable_addrs();
  addrs->set_place("广东深圳");
  addrs->set_num(1);

  person.set_health_status(model::HS_GOOD);

  // Note: The extension won't be translated to json
  std::string *school = person.MutableExtension(model::school);
  school->assign("华科大");
  person.SetExtension(model::learning_age, 20);

  std::string json;
  Code ret = proto::PBToJsonWithOutExtension(person, &json);
  EXPECT_EQ(kOk, ret);

  std::string expect_json =
      "{\"name\":\"lisi\",\"birthday\":1,\"is_student\":false,\"resv1\":\"good "
      "one\",\"resv2\":1234605616436508552,\"resv3\":-139292509886650761,\"addrs\":{\"place\":"
      "\"广东深圳\",\"num\":1},\"health_status\":101}";
  EXPECT_EQ(expect_json, json);
  // fprintf(stderr, "pb_str:\n%s\n", person.DebugString().c_str());
  // fprintf(stderr, "size:%zu, json:       %s\n", json.size(), json.c_str());
  // fprintf(stderr, "size:%zu, expect_json:%s\n", expect_json.size(), expect_json.c_str());

} /*}}}*/

TEST(PBToJsonWithOutExtension, Test_Normal_Person_Empty_Name) { /*{{{*/
  using namespace base;

  model::Person person;

  std::string json;
  Code ret = proto::PBToJsonWithOutExtension(person, &json);
  EXPECT_EQ(kOk, ret);

  std::string expect_json = "{\"name\":\"\"}";  // Note: Person.name is required
  EXPECT_EQ(expect_json, json);
  // fprintf(stderr, "pb_str:\n%s\n", person.DebugString().c_str());
  // fprintf(stderr, "size:%zu, json:       %s\n", json.size(), json.c_str());
  // fprintf(stderr, "size:%zu, expect_json:%s\n", expect_json.size(), expect_json.c_str());

} /*}}}*/

TEST(PBToJsonWithOutExtension, Test_Press_Person_Ten_Thousand) { /*{{{*/
  using namespace base;

  model::Person person;
  person.set_name("lisi");
  person.set_birthday(1);
  person.set_is_student(false);
  person.set_resv1("good one");
  person.set_resv2(0x1122334455667788);
  person.set_resv3(0xfe11223344556677);

  model::American *american = person.add_american_friends();
  american->set_id("american_1");
  american->set_name("jack_1");
  american->set_age(21);
  model::Addr *addr = american->mutable_addr();
  addr->set_place("New York");
  addr->set_num(1);
  model::Hobby *hobby = american->add_hobbies();
  hobby->set_name("swimming");
  hobby->set_skill_level(3);
  hobby = american->add_hobbies();
  hobby->set_name("chess");
  hobby->set_skill_level(4);
  hobby = american->add_hobbies();
  hobby->set_name("basketball");
  hobby->set_skill_level(4);

  american = person.add_american_friends();
  american->set_id("american_2");
  american->set_name("jack_2");
  american->set_age(22);

  american = person.add_american_friends();
  american->set_id("american_3");
  american->set_name("jack_3");
  american->set_age(23);

  model::English *english = person.add_english_friends();
  english->set_id("english_1");
  english->set_name("rose_1");
  english->set_age(21);

  english = person.add_english_friends();
  english->set_id("english_2");
  english->set_name("rose_2");
  english->set_age(22);

  english = person.add_english_friends();
  english->set_id("english_3");
  english->set_name("rose_3");
  english->set_age(23);

  person.set_health_status(model::HS_GOOD);

  std::string json;
  for (int i = 0; i < 10000; ++i) {
    json.clear();
    Code ret = proto::PBToJsonWithOutExtension(person, &json);
    EXPECT_EQ(kOk, ret);
  }

  std::string expect_json =
      "{\"name\":\"lisi\",\"birthday\":1,\"is_student\":false,\"resv1\":\"good "
      "one\",\"resv2\":1234605616436508552,\"resv3\":-139292509886650761,\"american_friends\":[{"
      "\"id\":\"american_1\",\"name\":\"jack_1\",\"age\":21,\"addr\":{\"place\":\"New "
      "York\",\"num\":1},\"hobbies\":[{\"name\":\"swimming\",\"skill_level\":3},{\"name\":"
      "\"chess\",\"skill_level\":4},{\"name\":\"basketball\",\"skill_level\":4}]},{\"id\":"
      "\"american_2\",\"name\":\"jack_2\",\"age\":22},{\"id\":\"american_3\",\"name\":\"jack_3\","
      "\"age\":23}],\"english_friends\":[{\"id\":\"english_1\",\"name\":\"rose_1\",\"age\":21},{"
      "\"id\":\"english_2\",\"name\":\"rose_2\",\"age\":22},{\"id\":\"english_3\",\"name\":\"rose_"
      "3\",\"age\":23}],\"health_status\":101}";
  EXPECT_EQ(expect_json, json);

  // fprintf(stderr, "pb_str:\n%s\n", person.DebugString().c_str());
  // fprintf(stderr, "size:%zu, json:       %s\n", json.size(), json.c_str());
  // fprintf(stderr, "size:%zu, expect_json:%s\n", expect_json.size(), expect_json.c_str());
} /*}}}*/

TEST(JsonToPBWithOutExtension, Test_Normal_Person) { /*{{{*/
  using namespace base;

  std::string source_json =
      "{\"name\":\"zhangsan\",\"age\":100,\"country_name\":\"China\",\"birthday\":1,\"height\":1."
      "73,\"weight\":105.41300201416016,\"is_student\":false,\"resv1\":\"good "
      "one\",\"resv2\":1234605616436508552,\"resv3\":-139292509886650761}";

  model::Person person;
  Code ret = proto::JsonToPBWithOutExtension(source_json, &person);
  EXPECT_EQ(kOk, ret);

  // fprintf(stderr, "pb_str:\n%s\n", person.DebugString().c_str());
  // fprintf(stderr, "size:%zu, json:       %s\n", source_json.size(), source_json.c_str());

  model::Person dest_person;
  dest_person.set_name("zhangsan");
  dest_person.set_age(100);
  dest_person.set_country_name("China");
  dest_person.set_birthday(1);
  dest_person.set_height(1.73);
  dest_person.set_weight(105.413);
  dest_person.set_is_student(false);
  dest_person.set_resv1("good one");
  dest_person.set_resv2(0x1122334455667788);
  dest_person.set_resv3(0xfe11223344556677);

  EXPECT_EQ(dest_person.DebugString(), person.DebugString());
} /*}}}*/

TEST(JsonToPBWithOutExtension, Test_Normal_Person_Enum) { /*{{{*/
  using namespace base;

  std::string source_json =
      "{\"name\":\"zhangsan\",\"birthday\":1,\"is_student\":false,\"resv1\":\"good "
      "one\",\"resv2\":1234605616436508552,\"resv3\":-139292509886650761,\"health_status\":102}";

  model::Person person;
  Code ret = proto::JsonToPBWithOutExtension(source_json, &person);
  EXPECT_EQ(kOk, ret);

  // fprintf(stderr, "pb_str:\n%s\n", person.DebugString().c_str());
  // fprintf(stderr, "size:%zu, json:       %s\n", source_json.size(), source_json.c_str());

  model::Person dest_person;
  dest_person.set_name("zhangsan");
  dest_person.set_birthday(1);
  dest_person.set_is_student(false);
  dest_person.set_resv1("good one");
  dest_person.set_resv2(0x1122334455667788);
  dest_person.set_resv3(0xfe11223344556677);
  dest_person.set_health_status(model::HS_MID);

  EXPECT_EQ(dest_person.DebugString(), person.DebugString());
} /*}}}*/

TEST(JsonToPBWithOutExtension, Test_Normal_Person_UpperField) { /*{{{*/
  using namespace base;

  std::string source_json =
      "{\"name\":\"æµè¯unicodeé "
      "·unicodeæµè¯\",\"birthday\":1,\"is_student\":false,\"resv1\":\"good "
      "one\",\"resv2\":1234605616436508552,\"resv3\":-139292509886650761,\"RESV4\":-"
      "139292509886650761,\"health_status\":102}";

  model::Person person;
  Code ret = proto::JsonToPBWithOutExtension(source_json, &person);
  EXPECT_EQ(kOk, ret);

  fprintf(stderr, "pb_str:\n%s\n", person.DebugString().c_str());
  fprintf(stderr, "size:%zu, json:       %s\n", source_json.size(), source_json.c_str());

  model::Person dest_person;
  dest_person.set_name("æµè¯unicodeé ·unicodeæµè¯");
  dest_person.set_birthday(1);
  dest_person.set_is_student(false);
  dest_person.set_resv1("good one");
  dest_person.set_resv2(0x1122334455667788);
  dest_person.set_resv3(0xfe11223344556677);
  dest_person.set_resv4(0xfe11223344556677);  // Note: upper field will be set of RESV4
  dest_person.set_health_status(model::HS_MID);

  EXPECT_EQ(dest_person.DebugString(), person.DebugString());
} /*}}}*/

TEST(JsonToPBWithOutExtension, Test_Normal_Person_Repeated_String) { /*{{{*/
  using namespace base;

  std::string source_json =
      "{\"name\":\"lisi\",\"birthday\":1,\"is_student\":false,\"resv1\":\"good "
      "one\",\"resv2\":1234605616436508552,\"resv3\":-139292509886650761,\"resv11\":[17,34,51,68],"
      "\"friends\":[\"Test1\",\"Test2\",\"Test3\",\"Test4\"],\"health_status\":101}";

  model::Person person;
  Code ret = proto::JsonToPBWithOutExtension(source_json, &person);
  EXPECT_EQ(kOk, ret);

  // fprintf(stderr, "pb_str:\n%s\n", person.DebugString().c_str());
  // fprintf(stderr, "size:%zu, json:       %s\n", source_json.size(), source_json.c_str());

  model::Person dest_person;
  dest_person.set_name("lisi");
  dest_person.set_birthday(1);
  dest_person.set_is_student(false);
  dest_person.set_resv1("good one");
  dest_person.set_resv2(0x1122334455667788);
  dest_person.set_resv3(0xfe11223344556677);

  dest_person.add_resv11(0x11);
  dest_person.add_resv11(0x22);
  dest_person.add_resv11(0x33);
  dest_person.add_resv11(0x44);

  dest_person.add_friends("Test1");
  dest_person.add_friends("Test2");
  dest_person.add_friends("Test3");
  dest_person.add_friends("Test4");

  dest_person.set_health_status(model::HS_GOOD);

  EXPECT_EQ(dest_person.DebugString(), person.DebugString());
} /*}}}*/

TEST(JsonToPBWithOutExtension, Test_Normal_Person_Repeated_OtherType) { /*{{{*/
  using namespace base;

  std::string source_json =
      "{\"name\":\"lisi\",\"birthday\":1,\"is_student\":false,\"resv1\":\"good "
      "one\",\"resv2\":1234605616436508552,\"resv3\":-139292509886650761,\"resv11\":[17,34,51,68],"
      "\"resv12\":[11.11,22.11,33.11,44.11],\"resv13\":[false,true,true,false],\"resv14\":["
      "\"adfas23243\",\"*lasdfnklwe\",\"热爱祖国\",\"好好学习，天天向上！！！\"],\"resv15\":[101,"
      "102,103,101],\"health_status\":101}";

  model::Person person;
  Code ret = proto::JsonToPBWithOutExtension(source_json, &person);
  EXPECT_EQ(kOk, ret);

  // fprintf(stderr, "pb_str:\n%s\n", person.DebugString().c_str());
  // fprintf(stderr, "size:%zu, json:       %s\n", source_json.size(), source_json.c_str());

  model::Person dest_person;
  dest_person.set_name("lisi");
  dest_person.set_birthday(1);
  dest_person.set_is_student(false);
  dest_person.set_resv1("good one");
  dest_person.set_resv2(0x1122334455667788);
  dest_person.set_resv3(0xfe11223344556677);

  dest_person.add_resv11(0x11);
  dest_person.add_resv11(0x22);
  dest_person.add_resv11(0x33);
  dest_person.add_resv11(0x44);

  dest_person.add_resv12(11.11);
  dest_person.add_resv12(22.11);
  dest_person.add_resv12(33.11);
  dest_person.add_resv12(44.11);

  dest_person.add_resv13(false);
  dest_person.add_resv13(true);
  dest_person.add_resv13(true);
  dest_person.add_resv13(false);

  dest_person.add_resv14("adfas23243");
  dest_person.add_resv14("*lasdfnklwe");
  dest_person.add_resv14("热爱祖国");
  dest_person.add_resv14("好好学习，天天向上！！！");

  dest_person.add_resv15(model::HS_GOOD);
  dest_person.add_resv15(model::HS_MID);
  dest_person.add_resv15(model::HS_BAD);
  dest_person.add_resv15(model::HS_GOOD);

  dest_person.set_health_status(model::HS_GOOD);

  EXPECT_EQ(dest_person.DebugString(), person.DebugString());
} /*}}}*/

TEST(JsonToPBWithOutExtension, Test_Normal_Person_Repeated_Object) { /*{{{*/
  using namespace base;

  std::string source_json =
      "{\"name\":\"lisi\",\"birthday\":1,\"is_student\":false,\"resv1\":\"good "
      "one\",\"resv2\":1234605616436508552,\"resv3\":-139292509886650761,\"american_friends\":[{"
      "\"id\":\"american_1\",\"name\":\"jack_1\",\"age\":21,\"addr\":{\"place\":\"New "
      "York\",\"num\":1},\"hobbies\":[{\"name\":\"swimming\",\"skill_level\":3},{\"name\":"
      "\"chess\",\"skill_level\":4},{\"name\":\"basketball\",\"skill_level\":4}]},{\"id\":"
      "\"american_2\",\"name\":\"jack_2\",\"age\":22},{\"id\":\"american_3\",\"name\":\"jack_3\","
      "\"age\":23}],\"english_friends\":[{\"id\":\"english_1\",\"name\":\"rose_1\",\"age\":21},{"
      "\"id\":\"english_2\",\"name\":\"rose_2\",\"age\":22},{\"id\":\"english_3\",\"name\":\"rose_"
      "3\",\"age\":23}],\"health_status\":101}";

  model::Person person;
  Code ret = proto::JsonToPBWithOutExtension(source_json, &person);
  EXPECT_EQ(kOk, ret);

  // fprintf(stderr, "pb_str:\n%s\n", person.DebugString().c_str());
  // fprintf(stderr, "size:%zu, json:       %s\n", source_json.size(), source_json.c_str());

  model::Person dest_person;
  dest_person.set_name("lisi");
  dest_person.set_birthday(1);
  dest_person.set_is_student(false);
  dest_person.set_resv1("good one");
  dest_person.set_resv2(0x1122334455667788);
  dest_person.set_resv3(0xfe11223344556677);

  model::American *american = dest_person.add_american_friends();
  american->set_id("american_1");
  american->set_name("jack_1");
  american->set_age(21);
  model::Addr *addr = american->mutable_addr();
  addr->set_place("New York");
  addr->set_num(1);
  model::Hobby *hobby = american->add_hobbies();
  hobby->set_name("swimming");
  hobby->set_skill_level(3);
  hobby = american->add_hobbies();
  hobby->set_name("chess");
  hobby->set_skill_level(4);
  hobby = american->add_hobbies();
  hobby->set_name("basketball");
  hobby->set_skill_level(4);

  american = dest_person.add_american_friends();
  american->set_id("american_2");
  american->set_name("jack_2");
  american->set_age(22);

  american = dest_person.add_american_friends();
  american->set_id("american_3");
  american->set_name("jack_3");
  american->set_age(23);

  model::English *english = dest_person.add_english_friends();
  english->set_id("english_1");
  english->set_name("rose_1");
  english->set_age(21);

  english = dest_person.add_english_friends();
  english->set_id("english_2");
  english->set_name("rose_2");
  english->set_age(22);

  english = dest_person.add_english_friends();
  english->set_id("english_3");
  english->set_name("rose_3");
  english->set_age(23);

  dest_person.set_health_status(model::HS_GOOD);

  EXPECT_EQ(dest_person.DebugString(), person.DebugString());
} /*}}}*/

TEST(JsonToPBWithOutExtension, Test_Normal_Person_Object) { /*{{{*/
  using namespace base;
  std::string source_json =
      "{\"name\":\"lisi\",\"birthday\":1,\"is_student\":false,\"resv1\":\"good "
      "one\",\"resv2\":1234605616436508552,\"resv3\":-139292509886650761,\"addrs\":{\"place\":"
      "\"广东深圳\",\"num\":1},\"health_status\":101}";

  model::Person person;
  Code ret = proto::JsonToPBWithOutExtension(source_json, &person);
  EXPECT_EQ(kOk, ret);

  // fprintf(stderr, "pb_str:\n%s\n", person.DebugString().c_str());
  // fprintf(stderr, "size:%zu, json:       %s\n", source_json.size(), source_json.c_str());

  model::Person dest_person;
  dest_person.set_name("lisi");
  dest_person.set_birthday(1);
  dest_person.set_is_student(false);
  dest_person.set_resv1("good one");
  dest_person.set_resv2(0x1122334455667788);
  dest_person.set_resv3(0xfe11223344556677);

  model::Addr *addrs = dest_person.mutable_addrs();
  addrs->set_place("广东深圳");
  addrs->set_num(1);

  dest_person.set_health_status(model::HS_GOOD);

  EXPECT_EQ(dest_person.DebugString(), person.DebugString());
} /*}}}*/

TEST(JsonToPBWithOutExtension, Test_Normal_Person_Object_Double_Same_Set) { /*{{{*/
  using namespace base;
  std::string source_json =
      "{\"name\":\"lisi\",\"birthday\":1,\"is_student\":false,\"resv1\":\"good "
      "one\",\"resv2\":1234605616436508552,\"resv3\":-139292509886650761,\"addrs\":{\"place\":"
      "\"广东深圳\",\"num\":1},\"health_status\":101}";

  model::Person person;
  person.set_name("lisi");
  person.set_birthday(1);
  person.set_is_student(false);
  person.set_resv1("good one");
  person.set_resv2(0x1122334455667788);
  person.set_resv3(0xfe11223344556677);
  model::Addr *person_addrs = person.mutable_addrs();
  person_addrs->set_place("广东深圳");
  person_addrs->set_num(1);

  Code ret = proto::JsonToPBWithOutExtension(source_json, &person);
  EXPECT_EQ(kOk, ret);

  // fprintf(stderr, "pb_str:\n%s\n", person.DebugString().c_str());
  // fprintf(stderr, "size:%zu, json:       %s\n", source_json.size(), source_json.c_str());

  model::Person dest_person;
  dest_person.set_name("lisi");
  dest_person.set_birthday(1);
  dest_person.set_is_student(false);
  dest_person.set_resv1("good one");
  dest_person.set_resv2(0x1122334455667788);
  dest_person.set_resv3(0xfe11223344556677);

  model::Addr *addrs = dest_person.mutable_addrs();
  addrs->set_place("广东深圳");
  addrs->set_num(1);

  dest_person.set_health_status(model::HS_GOOD);

  EXPECT_EQ(dest_person.DebugString(), person.DebugString());
} /*}}}*/

TEST(JsonToPBWithOutExtension, Test_Normal_Person_Object_Double_Diff_Set) { /*{{{*/
  using namespace base;
  std::string source_json =
      "{\"name\":\"lisi\",\"birthday\":1,\"is_student\":false,\"resv1\":\"good "
      "one\",\"resv2\":1234605616436508552,\"resv3\":-139292509886650761,\"addrs\":{\"place\":"
      "\"广东深圳\",\"num\":1},\"health_status\":101}";

  model::Person person;
  person.set_name("haha");
  person.set_birthday(2);
  person.set_is_student(true);
  person.set_resv1("just nice good one");
  person.set_resv2(0x873453455667788);
  person.set_resv3(0x873453344556677);
  model::Addr *person_addrs = person.mutable_addrs();
  person_addrs->set_place("河南郑州");
  person_addrs->set_num(2);

  Code ret = proto::JsonToPBWithOutExtension(source_json, &person);
  EXPECT_EQ(kOk, ret);

  // fprintf(stderr, "pb_str:\n%s\n", person.DebugString().c_str());
  // fprintf(stderr, "size:%zu, json:       %s\n", source_json.size(), source_json.c_str());

  model::Person dest_person;
  dest_person.set_name("lisi");
  dest_person.set_birthday(1);
  dest_person.set_is_student(false);
  dest_person.set_resv1("good one");
  dest_person.set_resv2(0x1122334455667788);
  dest_person.set_resv3(0xfe11223344556677);

  model::Addr *addrs = dest_person.mutable_addrs();
  addrs->set_place("广东深圳");
  addrs->set_num(1);

  dest_person.set_health_status(model::HS_GOOD);

  EXPECT_EQ(dest_person.DebugString(), person.DebugString());
} /*}}}*/

TEST(JsonToPBWithOutExtension, Test_Normal_Person_Extension) { /*{{{*/
  using namespace base;

  std::string source_json =
      "{\"name\":\"lisi\",\"birthday\":1,\"is_student\":false,\"resv1\":\"good "
      "one\",\"resv2\":1234605616436508552,\"resv3\":-139292509886650761,\"addrs\":{\"place\":"
      "\"广东深圳\",\"num\":1},\"health_status\":101}";

  model::Person person;
  Code ret = proto::JsonToPBWithOutExtension(source_json, &person);
  EXPECT_EQ(kOk, ret);

  // fprintf(stderr, "pb_str:\n%s\n", person.DebugString().c_str());
  // fprintf(stderr, "size:%zu, json:       %s\n", source_json.size(), source_json.c_str());

  model::Person dest_person;
  dest_person.set_name("lisi");
  dest_person.set_birthday(1);
  dest_person.set_is_student(false);
  dest_person.set_resv1("good one");
  dest_person.set_resv2(0x1122334455667788);
  dest_person.set_resv3(0xfe11223344556677);

  model::Addr *addrs = dest_person.mutable_addrs();
  addrs->set_place("广东深圳");
  addrs->set_num(1);

  dest_person.set_health_status(model::HS_GOOD);

  // Note: The extension won't be translated to json
  // std::string *school = dest_person.MutableExtension(model::school);
  // school->assign("华科大");
  // dest_person.SetExtension(model::learning_age, 20);

  EXPECT_EQ(dest_person.DebugString(), person.DebugString());
} /*}}}*/

TEST(JsonToPBWithOutExtension, Test_Normal_Person_Empty_Name) { /*{{{*/
  using namespace base;

  std::string source_json = "{\"name\":\"\"}";  // Note: Person.name is required

  model::Person person;
  Code ret = proto::JsonToPBWithOutExtension(source_json, &person);
  EXPECT_EQ(kOk, ret);

  // fprintf(stderr, "pb_str:\n%s\n", person.DebugString().c_str());
  // fprintf(stderr, "size:%zu, json:       %s\n", source_json.size(), source_json.c_str());

  model::Person dest_person;
  dest_person.set_name("");

  EXPECT_EQ(dest_person.DebugString(), person.DebugString());
} /*}}}*/

TEST(JsonToPBWithOutExtension, Test_Normal_Person_StringEnum) { /*{{{*/
  using namespace base;

  std::string source_json =
      "{\"name\":\"zhangsan\",\"birthday\":1,\"is_student\":false,\"resv1\":\"good "
      "one\",\"resv2\":1234605616436508552,\"resv3\":-139292509886650761,\"health_status\":\"HS_"
      "MID\"}";

  model::Person person;
  Code ret = proto::JsonToPBWithOutExtension(source_json, &person);
  EXPECT_EQ(kOk, ret);

  // fprintf(stderr, "pb_str:\n%s\n", person.DebugString().c_str());
  // fprintf(stderr, "size:%zu, json:       %s\n", source_json.size(), source_json.c_str());

  model::Person dest_person;
  dest_person.set_name("zhangsan");
  dest_person.set_birthday(1);
  dest_person.set_is_student(false);
  dest_person.set_resv1("good one");
  dest_person.set_resv2(0x1122334455667788);
  dest_person.set_resv3(0xfe11223344556677);
  dest_person.set_health_status(model::HS_MID);

  EXPECT_EQ(dest_person.DebugString(), person.DebugString());
} /*}}}*/

TEST(JsonToPBWithOutExtension, Test_Press_Person_Ten_Thousand) { /*{{{*/
  using namespace base;

  std::string source_json =
      "{\"name\":\"lisi\",\"birthday\":1,\"is_student\":false,\"resv1\":\"good "
      "one\",\"resv2\":1234605616436508552,\"resv3\":-139292509886650761,\"american_friends\":[{"
      "\"id\":\"american_1\",\"name\":\"jack_1\",\"age\":21,\"addr\":{\"place\":\"New "
      "York\",\"num\":1},\"hobbies\":[{\"name\":\"swimming\",\"skill_level\":3},{\"name\":"
      "\"chess\",\"skill_level\":4},{\"name\":\"basketball\",\"skill_level\":4}]},{\"id\":"
      "\"american_2\",\"name\":\"jack_2\",\"age\":22},{\"id\":\"american_3\",\"name\":\"jack_3\","
      "\"age\":23}],\"english_friends\":[{\"id\":\"english_1\",\"name\":\"rose_1\",\"age\":21},{"
      "\"id\":\"english_2\",\"name\":\"rose_2\",\"age\":22},{\"id\":\"english_3\",\"name\":\"rose_"
      "3\",\"age\":23}],\"health_status\":101}";

  model::Person person;
  for (int i = 0; i < 10000; ++i) {
    person.Clear();
    Code ret = proto::JsonToPBWithOutExtension(source_json, &person);
    EXPECT_EQ(kOk, ret);
  }

  // fprintf(stderr, "pb_str:\n%s\n", person.DebugString().c_str());
  // fprintf(stderr, "size:%zu, json:       %s\n", source_json.size(), source_json.c_str());

  model::Person dest_person;
  dest_person.set_name("lisi");
  dest_person.set_birthday(1);
  dest_person.set_is_student(false);
  dest_person.set_resv1("good one");
  dest_person.set_resv2(0x1122334455667788);
  dest_person.set_resv3(0xfe11223344556677);

  model::American *american = dest_person.add_american_friends();
  american->set_id("american_1");
  american->set_name("jack_1");
  american->set_age(21);
  model::Addr *addr = american->mutable_addr();
  addr->set_place("New York");
  addr->set_num(1);
  model::Hobby *hobby = american->add_hobbies();
  hobby->set_name("swimming");
  hobby->set_skill_level(3);
  hobby = american->add_hobbies();
  hobby->set_name("chess");
  hobby->set_skill_level(4);
  hobby = american->add_hobbies();
  hobby->set_name("basketball");
  hobby->set_skill_level(4);

  american = dest_person.add_american_friends();
  american->set_id("american_2");
  american->set_name("jack_2");
  american->set_age(22);

  american = dest_person.add_american_friends();
  american->set_id("american_3");
  american->set_name("jack_3");
  american->set_age(23);

  model::English *english = dest_person.add_english_friends();
  english->set_id("english_1");
  english->set_name("rose_1");
  english->set_age(21);

  english = dest_person.add_english_friends();
  english->set_id("english_2");
  english->set_name("rose_2");
  english->set_age(22);

  english = dest_person.add_english_friends();
  english->set_id("english_3");
  english->set_name("rose_3");
  english->set_age(23);

  dest_person.set_health_status(model::HS_GOOD);

  EXPECT_EQ(dest_person.DebugString(), person.DebugString());
} /*}}}*/

TEST(JsonToPBWithOutExtension, Test_Exception_Person_Object_More_Field_Set) { /*{{{*/
  using namespace base;
  std::string source_json =
      "{\"name\":\"lisi\",\"birthday\":1,\"is_student\":false,\"resv1\":\"good "
      "one\",\"resv2\":1234605616436508552,\"resv3\":-139292509886650761,\"addrs\":{\"place\":"
      "\"广东深圳\",\"num\":1},\"health_status\":101}";

  model::Person person;
  person.set_weight(1.03);

  Code ret = proto::JsonToPBWithOutExtension(source_json, &person);
  EXPECT_EQ(kOk, ret);

  // fprintf(stderr, "pb_str:\n%s\n", person.DebugString().c_str());
  // fprintf(stderr, "size:%zu, json:       %s\n", source_json.size(), source_json.c_str());

  model::Person dest_person;
  dest_person.set_name("lisi");
  dest_person.set_birthday(1);
  dest_person.set_is_student(false);
  dest_person.set_resv1("good one");
  dest_person.set_resv2(0x1122334455667788);
  dest_person.set_resv3(0xfe11223344556677);

  model::Addr *addrs = dest_person.mutable_addrs();
  addrs->set_place("广东深圳");
  addrs->set_num(1);

  dest_person.set_health_status(model::HS_GOOD);

  EXPECT_NEQ(dest_person.DebugString(), person.DebugString());
} /*}}}*/

TEST(JsonToPBWithOutExtension, Test_Exception_Person_InvalidNumEnum) { /*{{{*/
  using namespace base;

  std::string source_json =
      "{\"name\":\"zhangsan\",\"birthday\":1,\"is_student\":false,\"resv1\":\"good "
      "one\",\"resv2\":1234605616436508552,\"resv3\":-139292509886650761,\"health_status\":111111}";

  model::Person person;
  Code ret = proto::JsonToPBWithOutExtension(source_json, &person);
  EXPECT_EQ(base::kInvalidParam, ret);
} /*}}}*/

TEST(JsonToPBWithOutExtension, Test_Exception_Person_InvalidStringEnum) { /*{{{*/
  using namespace base;

  std::string source_json =
      "{\"name\":\"zhangsan\",\"birthday\":1,\"is_student\":false,\"resv1\":\"good "
      "one\",\"resv2\":1234605616436508552,\"resv3\":-139292509886650761,\"health_status\":\"JUST_"
      "ERROR\"}";

  model::Person person;
  Code ret = proto::JsonToPBWithOutExtension(source_json, &person);
  EXPECT_EQ(base::kInvalidParam, ret);
} /*}}}*/

TEST(JsonToPBWithOutExtension, Test_Exception_Person_InvalidTypeEnum) { /*{{{*/
  using namespace base;

  std::string source_json =
      "{\"name\":\"zhangsan\",\"birthday\":1,\"is_student\":false,\"resv1\":\"good "
      "one\",\"resv2\":1234605616436508552,\"resv3\":-139292509886650761,\"health_status\":true}";

  model::Person person;
  Code ret = proto::JsonToPBWithOutExtension(source_json, &person);
  EXPECT_EQ(base::kInvalidParam, ret);
} /*}}}*/

TEST(JsonToPBWithOutExtension, Test_Exception_Person_Repeated_InvalidNumEnum) { /*{{{*/
  using namespace base;

  std::string source_json =
      "{\"name\":\"lisi\",\"birthday\":1,\"is_student\":false,\"resv1\":\"good "
      "one\",\"resv2\":1234605616436508552,\"resv3\":-139292509886650761,\"resv11\":[17,34,51,68],"
      "\"resv12\":[11.11,22.11,33.11,44.11],\"resv13\":[false,true,true,false],\"resv14\":["
      "\"adfas23243\",\"*lasdfnklwe\",\"热爱祖国\",\"好好学习，天天向上！！！\"],\"resv15\":["
      "1111111,102,103,101],\"health_status\":101}";

  model::Person person;
  Code ret = proto::JsonToPBWithOutExtension(source_json, &person);
  EXPECT_EQ(base::kInvalidParam, ret);
} /*}}}*/

TEST(JsonToPBWithOutExtension, Test_Exception_Person_Repeated_InvalidStringEnum) { /*{{{*/
  using namespace base;

  std::string source_json =
      "{\"name\":\"lisi\",\"birthday\":1,\"is_student\":false,\"resv1\":\"good "
      "one\",\"resv2\":1234605616436508552,\"resv3\":-139292509886650761,\"resv11\":[17,34,51,68],"
      "\"resv12\":[11.11,22.11,33.11,44.11],\"resv13\":[false,true,true,false],\"resv14\":["
      "\"adfas23243\",\"*lasdfnklwe\",\"热爱祖国\",\"好好学习，天天向上！！！\"],\"resv15\":["
      "\"iiiijjj\"],\"health_status\":101}";

  model::Person person;
  Code ret = proto::JsonToPBWithOutExtension(source_json, &person);
  EXPECT_EQ(base::kInvalidParam, ret);
} /*}}}*/

TEST(JsonToPBWithOutExtension, Test_Exception_Person_Repeated_InvalidTypeEnum) { /*{{{*/
  using namespace base;

  std::string source_json =
      "{\"name\":\"lisi\",\"birthday\":1,\"is_student\":false,\"resv1\":\"good "
      "one\",\"resv2\":1234605616436508552,\"resv3\":-139292509886650761,\"resv11\":[17,34,51,68],"
      "\"resv12\":[11.11,22.11,33.11,44.11],\"resv13\":[false,true,true,false],\"resv14\":["
      "\"adfas23243\",\"*lasdfnklwe\",\"热爱祖国\",\"好好学习，天天向上！！！\"],\"resv15\":[false,"
      " true],\"health_status\":101}";

  model::Person person;
  Code ret = proto::JsonToPBWithOutExtension(source_json, &person);
  EXPECT_EQ(base::kInvalidParam, ret);
} /*}}}*/

TEST(JsonToPBWithOutExtension, Test_Exception_Person_InvalidIntType) { /*{{{*/
  using namespace base;

  std::string source_json =
      "{\"name\":\"zhangsan\",\"birthday\":\"ERROR "
      "BIRTHDAY\",\"is_student\":false,\"resv1\":\"good "
      "one\",\"resv2\":1234605616436508552,\"resv3\":-139292509886650761,\"health_status\":111111}";

  model::Person person;
  Code ret = proto::JsonToPBWithOutExtension(source_json, &person);
  EXPECT_EQ(base::kInvalidParam, ret);
} /*}}}*/

TEST(JsonToPBWithOutExtension, Test_Exception_Person_InvalidDoubleType) { /*{{{*/
  using namespace base;

  std::string source_json =
      "{\"name\":\"zhangsan\",\"birthday\":1,\"height\":\"AAAAAA\",\"is_student\":false,\"resv1\":"
      "\"good "
      "one\",\"resv2\":1234605616436508552,\"resv3\":-139292509886650761,\"health_status\":111111}";

  model::Person person;
  Code ret = proto::JsonToPBWithOutExtension(source_json, &person);
  EXPECT_EQ(base::kInvalidParam, ret);
} /*}}}*/

TEST(JsonToPBWithOutExtension, Test_Exception_Person_InvalidBoolType) { /*{{{*/
  using namespace base;

  std::string source_json =
      "{\"name\":\"zhangsan\",\"birthday\":1,\"is_student\":1,\"resv1\":\"good "
      "one\",\"resv2\":1234605616436508552,\"resv3\":-139292509886650761,\"health_status\":111111}";

  model::Person person;
  Code ret = proto::JsonToPBWithOutExtension(source_json, &person);
  EXPECT_EQ(base::kInvalidParam, ret);
} /*}}}*/

TEST(JsonToPBWithOutExtension, Test_Exception_Person_InvalidStringType) { /*{{{*/
  using namespace base;

  std::string source_json =
      "{\"name\":true,\"birthday\":1,\"is_student\":false,\"resv1\":\"good "
      "one\",\"resv2\":1234605616436508552,\"resv3\":-139292509886650761,\"health_status\":111111}";

  model::Person person;
  Code ret = proto::JsonToPBWithOutExtension(source_json, &person);
  EXPECT_EQ(base::kInvalidParam, ret);
} /*}}}*/

TEST(JsonToPBWithOutExtension, Test_Exception_Person_InvalidField) { /*{{{*/
  using namespace base;

  std::string source_json =
      "{\"XXX\":\"zhangsan\",\"birthday\":1,\"resv1\":\"good "
      "one\",\"resv2\":1234605616436508552,\"resv3\":-139292509886650761,\"health_status\":111111}";

  model::Person person;
  Code ret = proto::JsonToPBWithOutExtension(source_json, &person);
  EXPECT_EQ(base::kInvalidParam, ret);
} /*}}}*/

TEST(JsonToPBWithOutExtension, Test_Exception_Person_Empty) { /*{{{*/
  using namespace base;

  std::string source_json = "";

  model::Person person;
  Code ret = proto::JsonToPBWithOutExtension(source_json, &person);
  EXPECT_EQ(base::kInvalidParam, ret);
} /*}}}*/

TEST(PBMange, Test_Normal_Person) { /*{{{*/
  using namespace base;

  std::string source_json =
      "{\"name\":\"zhangsan\",\"age\":100,\"country_name\":\"China\",\"birthday\":1,\"height\":1."
      "73,\"weight\":105.41300201416016,\"is_student\":false,\"resv1\":\"good "
      "one\",\"resv2\":1234605616436508552,\"resv3\":-139292509886650761}";

  model::Person person;
  Code ret = proto::JsonToPBWithOutExtension(source_json, &person);
  EXPECT_EQ(kOk, ret);

  // fprintf(stderr, "pb_str:\n%s\n", person.DebugString().c_str());
  // fprintf(stderr, "size:%zu, json:       %s\n", source_json.size(), source_json.c_str());

  std::string source_person_str;
  EXPECT_EQ(true, person.SerializeToString(&source_person_str));

  // NOTE: read from pb file;
  std::string pb_dir = "../protobuf";
  std::string pb_file_name = "model.proto";
  std::vector<std::string> pb_type_names;
  pb_type_names.push_back("Person");

  proto::PBManage pb_manage(pb_dir, pb_file_name);
  ret = pb_manage.Init(pb_type_names);
  EXPECT_EQ(kOk, ret);

  google::protobuf::Message *ref_person = pb_manage.NewMsg("Person");
  EXPECT_NEQ(NULL, ref_person);

  EXPECT_EQ(true, ref_person->ParseFromString(source_person_str));
  fprintf(stderr, "person:%s\n", ref_person->DebugString().c_str());
  EXPECT_EQ(person.DebugString(), ref_person->DebugString());

  delete ref_person;
} /*}}}*/

TEST(PBMange, Test_Normal_Person_Enum) { /*{{{*/
  using namespace base;

  std::string source_json =
      "{\"name\":\"zhangsan\",\"birthday\":1,\"is_student\":false,\"resv1\":\"good "
      "one\",\"resv2\":1234605616436508552,\"resv3\":-139292509886650761,\"health_status\":102}";

  model::Person person;
  Code ret = proto::JsonToPBWithOutExtension(source_json, &person);
  EXPECT_EQ(kOk, ret);

  // fprintf(stderr, "pb_str:\n%s\n", person.DebugString().c_str());
  // fprintf(stderr, "size:%zu, json:       %s\n", source_json.size(), source_json.c_str());

  std::string source_person_str;
  EXPECT_EQ(true, person.SerializeToString(&source_person_str));

  // NOTE: read from pb file;
  std::string pb_dir = "../protobuf";
  std::string pb_file_name = "model.proto";
  std::vector<std::string> pb_type_names;
  pb_type_names.push_back("Person");

  proto::PBManage pb_manage(pb_dir, pb_file_name);
  ret = pb_manage.Init(pb_type_names);
  EXPECT_EQ(kOk, ret);

  google::protobuf::Message *ref_person = pb_manage.NewMsg("Person");
  EXPECT_NEQ(NULL, ref_person);

  EXPECT_EQ(true, ref_person->ParseFromString(source_person_str));
  fprintf(stderr, "person:%s\n", ref_person->DebugString().c_str());
  EXPECT_EQ(person.DebugString(), ref_person->DebugString());

  delete ref_person;
} /*}}}*/

TEST(PBMange, Test_Normal_Person_UpperField) { /*{{{*/
  using namespace base;

  std::string source_json =
      "{\"name\":\"zhangsan\",\"birthday\":1,\"is_student\":false,\"resv1\":\"good "
      "one\",\"resv2\":1234605616436508552,\"resv3\":-139292509886650761,\"RESV4\":-"
      "139292509886650761,\"health_status\":102}";

  model::Person person;
  Code ret = proto::JsonToPBWithOutExtension(source_json, &person);
  EXPECT_EQ(kOk, ret);

  // fprintf(stderr, "pb_str:\n%s\n", person.DebugString().c_str());
  // fprintf(stderr, "size:%zu, json:       %s\n", source_json.size(), source_json.c_str());

  std::string source_person_str;
  EXPECT_EQ(true, person.SerializeToString(&source_person_str));

  // NOTE: read from pb file;
  std::string pb_dir = "../protobuf";
  std::string pb_file_name = "model.proto";
  std::vector<std::string> pb_type_names;
  pb_type_names.push_back("Person");

  proto::PBManage pb_manage(pb_dir, pb_file_name);
  ret = pb_manage.Init(pb_type_names);
  EXPECT_EQ(kOk, ret);

  google::protobuf::Message *ref_person = pb_manage.NewMsg("Person");
  EXPECT_NEQ(NULL, ref_person);

  EXPECT_EQ(true, ref_person->ParseFromString(source_person_str));
  fprintf(stderr, "person:%s\n", ref_person->DebugString().c_str());
  EXPECT_EQ(person.DebugString(), ref_person->DebugString());

  delete ref_person;
} /*}}}*/

TEST(PBMange, Test_Normal_Person_Repeated_String) { /*{{{*/
  using namespace base;

  std::string source_json =
      "{\"name\":\"lisi\",\"birthday\":1,\"is_student\":false,\"resv1\":\"good "
      "one\",\"resv2\":1234605616436508552,\"resv3\":-139292509886650761,\"resv11\":[17,34,51,68],"
      "\"friends\":[\"Test1\",\"Test2\",\"Test3\",\"Test4\"],\"health_status\":101}";

  model::Person person;
  Code ret = proto::JsonToPBWithOutExtension(source_json, &person);
  EXPECT_EQ(kOk, ret);

  // fprintf(stderr, "pb_str:\n%s\n", person.DebugString().c_str());
  // fprintf(stderr, "size:%zu, json:       %s\n", source_json.size(), source_json.c_str());

  std::string source_person_str;
  EXPECT_EQ(true, person.SerializeToString(&source_person_str));

  // NOTE: read from pb file;
  std::string pb_dir = "../protobuf";
  std::string pb_file_name = "model.proto";
  std::vector<std::string> pb_type_names;
  pb_type_names.push_back("Person");

  proto::PBManage pb_manage(pb_dir, pb_file_name);
  ret = pb_manage.Init(pb_type_names);
  EXPECT_EQ(kOk, ret);

  google::protobuf::Message *ref_person = pb_manage.NewMsg("Person");
  EXPECT_NEQ(NULL, ref_person);

  EXPECT_EQ(true, ref_person->ParseFromString(source_person_str));
  fprintf(stderr, "person:%s\n", ref_person->DebugString().c_str());
  EXPECT_EQ(person.DebugString(), ref_person->DebugString());

  delete ref_person;
} /*}}}*/

TEST(PBMange, Test_Normal_Person_Repeated_OtherType) { /*{{{*/
  using namespace base;

  std::string source_json =
      "{\"name\":\"lisi\",\"birthday\":1,\"is_student\":false,\"resv1\":\"good "
      "one\",\"resv2\":1234605616436508552,\"resv3\":-139292509886650761,\"resv11\":[17,34,51,68],"
      "\"resv12\":[11.11,22.11,33.11,44.11],\"resv13\":[false,true,true,false],\"resv14\":["
      "\"adfas23243\",\"*lasdfnklwe\",\"热爱祖国\",\"好好学习，天天向上！！！\"],\"resv15\":[101,"
      "102,103,101],\"health_status\":101}";

  model::Person person;
  Code ret = proto::JsonToPBWithOutExtension(source_json, &person);
  EXPECT_EQ(kOk, ret);

  // fprintf(stderr, "pb_str:\n%s\n", person.DebugString().c_str());
  // fprintf(stderr, "size:%zu, json:       %s\n", source_json.size(), source_json.c_str());

  std::string source_person_str;
  EXPECT_EQ(true, person.SerializeToString(&source_person_str));

  // NOTE: read from pb file;
  std::string pb_dir = "../protobuf";
  std::string pb_file_name = "model.proto";
  std::vector<std::string> pb_type_names;
  pb_type_names.push_back("Person");

  proto::PBManage pb_manage(pb_dir, pb_file_name);
  ret = pb_manage.Init(pb_type_names);
  EXPECT_EQ(kOk, ret);

  google::protobuf::Message *ref_person = pb_manage.NewMsg("Person");
  EXPECT_NEQ(NULL, ref_person);

  EXPECT_EQ(true, ref_person->ParseFromString(source_person_str));
  fprintf(stderr, "person:%s\n", ref_person->DebugString().c_str());
  EXPECT_EQ(person.DebugString(), ref_person->DebugString());

  delete ref_person;
} /*}}}*/

TEST(PBMange, Test_Normal_Person_Repeated_Object) { /*{{{*/
  using namespace base;

  std::string source_json =
      "{\"name\":\"lisi\",\"birthday\":1,\"is_student\":false,\"resv1\":\"good "
      "one\",\"resv2\":1234605616436508552,\"resv3\":-139292509886650761,\"american_friends\":[{"
      "\"id\":\"american_1\",\"name\":\"jack_1\",\"age\":21,\"addr\":{\"place\":\"New "
      "York\",\"num\":1},\"hobbies\":[{\"name\":\"swimming\",\"skill_level\":3},{\"name\":"
      "\"chess\",\"skill_level\":4},{\"name\":\"basketball\",\"skill_level\":4}]},{\"id\":"
      "\"american_2\",\"name\":\"jack_2\",\"age\":22},{\"id\":\"american_3\",\"name\":\"jack_3\","
      "\"age\":23}],\"english_friends\":[{\"id\":\"english_1\",\"name\":\"rose_1\",\"age\":21},{"
      "\"id\":\"english_2\",\"name\":\"rose_2\",\"age\":22},{\"id\":\"english_3\",\"name\":\"rose_"
      "3\",\"age\":23}],\"health_status\":101}";

  model::Person person;
  Code ret = proto::JsonToPBWithOutExtension(source_json, &person);
  EXPECT_EQ(kOk, ret);

  // fprintf(stderr, "pb_str:\n%s\n", person.DebugString().c_str());
  // fprintf(stderr, "size:%zu, json:       %s\n", source_json.size(), source_json.c_str());

  std::string source_person_str;
  EXPECT_EQ(true, person.SerializeToString(&source_person_str));

  // NOTE: read from pb file;
  std::string pb_dir = "../protobuf";
  std::string pb_file_name = "model.proto";
  std::vector<std::string> pb_type_names;
  pb_type_names.push_back("Person");

  proto::PBManage pb_manage(pb_dir, pb_file_name);
  ret = pb_manage.Init(pb_type_names);
  EXPECT_EQ(kOk, ret);

  google::protobuf::Message *ref_person = pb_manage.NewMsg("Person");
  EXPECT_NEQ(NULL, ref_person);

  EXPECT_EQ(true, ref_person->ParseFromString(source_person_str));
  fprintf(stderr, "person:%s\n", ref_person->DebugString().c_str());
  EXPECT_EQ(person.DebugString(), ref_person->DebugString());

  delete ref_person;
} /*}}}*/

TEST(PBMange, Test_Normal_Person_Object) { /*{{{*/
  using namespace base;

  std::string source_json =
      "{\"name\":\"lisi\",\"birthday\":1,\"is_student\":false,\"resv1\":\"good "
      "one\",\"resv2\":1234605616436508552,\"resv3\":-139292509886650761,\"addrs\":{\"place\":"
      "\"广东深圳\",\"num\":1},\"health_status\":101}";

  model::Person person;
  Code ret = proto::JsonToPBWithOutExtension(source_json, &person);
  EXPECT_EQ(kOk, ret);

  // fprintf(stderr, "pb_str:\n%s\n", person.DebugString().c_str());
  // fprintf(stderr, "size:%zu, json:       %s\n", source_json.size(), source_json.c_str());

  std::string source_person_str;
  EXPECT_EQ(true, person.SerializeToString(&source_person_str));

  // NOTE: read from pb file;
  std::string pb_dir = "../protobuf";
  std::string pb_file_name = "model.proto";
  std::vector<std::string> pb_type_names;
  pb_type_names.push_back("Person");

  proto::PBManage pb_manage(pb_dir, pb_file_name);
  ret = pb_manage.Init(pb_type_names);
  EXPECT_EQ(kOk, ret);

  google::protobuf::Message *ref_person = pb_manage.NewMsg("Person");
  EXPECT_NEQ(NULL, ref_person);

  EXPECT_EQ(true, ref_person->ParseFromString(source_person_str));
  fprintf(stderr, "person:%s\n", ref_person->DebugString().c_str());
  EXPECT_EQ(person.DebugString(), ref_person->DebugString());

  delete ref_person;
} /*}}}*/

TEST(PBMange, Test_Normal_Person_Extension) { /*{{{*/
  using namespace base;

  std::string source_json =
      "{\"name\":\"lisi\",\"birthday\":1,\"is_student\":false,\"resv1\":\"good "
      "one\",\"resv2\":1234605616436508552,\"resv3\":-139292509886650761,\"addrs\":{\"place\":"
      "\"广东深圳\",\"num\":1},\"health_status\":101}";

  model::Person person;
  Code ret = proto::JsonToPBWithOutExtension(source_json, &person);
  EXPECT_EQ(kOk, ret);

  // fprintf(stderr, "pb_str:\n%s\n", person.DebugString().c_str());
  // fprintf(stderr, "size:%zu, json:       %s\n", source_json.size(), source_json.c_str());

  std::string source_person_str;
  EXPECT_EQ(true, person.SerializeToString(&source_person_str));

  // NOTE: read from pb file;
  std::string pb_dir = "../protobuf";
  std::string pb_file_name = "model.proto";
  std::vector<std::string> pb_type_names;
  pb_type_names.push_back("Person");

  proto::PBManage pb_manage(pb_dir, pb_file_name);
  ret = pb_manage.Init(pb_type_names);
  EXPECT_EQ(kOk, ret);

  google::protobuf::Message *ref_person = pb_manage.NewMsg("Person");
  EXPECT_NEQ(NULL, ref_person);

  EXPECT_EQ(true, ref_person->ParseFromString(source_person_str));
  fprintf(stderr, "person:%s\n", ref_person->DebugString().c_str());
  EXPECT_EQ(person.DebugString(), ref_person->DebugString());

  delete ref_person;
} /*}}}*/

TEST(PBMange, Test_Normal_Person_Empty_Name) { /*{{{*/
  using namespace base;

  std::string source_json = "{\"name\":\"\"}";  // Note: Person.name is required

  model::Person person;
  Code ret = proto::JsonToPBWithOutExtension(source_json, &person);
  EXPECT_EQ(kOk, ret);

  // fprintf(stderr, "pb_str:\n%s\n", person.DebugString().c_str());
  // fprintf(stderr, "size:%zu, json:       %s\n", source_json.size(), source_json.c_str());

  std::string source_person_str;
  EXPECT_EQ(true, person.SerializeToString(&source_person_str));

  // NOTE: read from pb file;
  std::string pb_dir = "../protobuf";
  std::string pb_file_name = "model.proto";
  std::vector<std::string> pb_type_names;
  pb_type_names.push_back("Person");

  proto::PBManage pb_manage(pb_dir, pb_file_name);
  ret = pb_manage.Init(pb_type_names);
  EXPECT_EQ(kOk, ret);

  google::protobuf::Message *ref_person = pb_manage.NewMsg("Person");
  EXPECT_NEQ(NULL, ref_person);

  EXPECT_EQ(true, ref_person->ParseFromString(source_person_str));
  fprintf(stderr, "person:%s\n", ref_person->DebugString().c_str());
  EXPECT_EQ(person.DebugString(), ref_person->DebugString());

  delete ref_person;
} /*}}}*/

TEST(PBMange, Test_Normal_Person_StringEnum) { /*{{{*/
  using namespace base;

  std::string source_json =
      "{\"name\":\"zhangsan\",\"birthday\":1,\"is_student\":false,\"resv1\":\"good "
      "one\",\"resv2\":1234605616436508552,\"resv3\":-139292509886650761,\"health_status\":\"HS_"
      "MID\"}";

  model::Person person;
  Code ret = proto::JsonToPBWithOutExtension(source_json, &person);
  EXPECT_EQ(kOk, ret);

  // fprintf(stderr, "pb_str:\n%s\n", person.DebugString().c_str());
  // fprintf(stderr, "size:%zu, json:       %s\n", source_json.size(), source_json.c_str());

  std::string source_person_str;
  EXPECT_EQ(true, person.SerializeToString(&source_person_str));

  // NOTE: read from pb file;
  std::string pb_dir = "../protobuf";
  std::string pb_file_name = "model.proto";
  std::vector<std::string> pb_type_names;
  pb_type_names.push_back("Person");

  proto::PBManage pb_manage(pb_dir, pb_file_name);
  ret = pb_manage.Init(pb_type_names);
  EXPECT_EQ(kOk, ret);

  google::protobuf::Message *ref_person = pb_manage.NewMsg("Person");
  EXPECT_NEQ(NULL, ref_person);

  EXPECT_EQ(true, ref_person->ParseFromString(source_person_str));
  fprintf(stderr, "person:%s\n", ref_person->DebugString().c_str());
  EXPECT_EQ(person.DebugString(), ref_person->DebugString());

  delete ref_person;
} /*}}}*/

TEST(PBMange, Test_Normal_Two_Manage) { /*{{{*/
  using namespace base;

  std::string source_json =
      "{\"name\":\"lisi\",\"birthday\":1,\"is_student\":false,\"resv1\":\"good "
      "one\",\"resv2\":1234605616436508552,\"resv3\":-139292509886650761,\"addrs\":{\"place\":"
      "\"广东深圳\",\"num\":1},\"health_status\":101}";

  model::Person person;
  Code ret = proto::JsonToPBWithOutExtension(source_json, &person);
  EXPECT_EQ(kOk, ret);

  std::string source_person_str;
  EXPECT_EQ(true, person.SerializeToString(&source_person_str));

  // NOTE: read from pb file;
  std::string pb_dir = "../protobuf";
  std::string pb_file_name = "model.proto";
  std::vector<std::string> pb_type_names;
  pb_type_names.push_back("Person");

  proto::PBManage pb_manage(pb_dir, pb_file_name);
  ret = pb_manage.Init(pb_type_names);
  EXPECT_EQ(kOk, ret);

  google::protobuf::Message *ref_person = pb_manage.NewMsg("Person");
  EXPECT_NEQ(NULL, ref_person);

  EXPECT_EQ(true, ref_person->ParseFromString(source_person_str));
  fprintf(stderr, "person:%s\n", ref_person->DebugString().c_str());
  EXPECT_EQ(person.DebugString(), ref_person->DebugString());
  delete ref_person;

  // NOTE: model_2
  std::string source_json_2 = "{\"name\":\"fish\",\"place\":\"china\",\"age\":10}";
  std::string pb_dir_2 = "../protobuf";
  std::string pb_file_name_2 = "model_2.proto";
  std::vector<std::string> pb_type_names_2;
  pb_type_names_2.push_back("Animal");

  proto::PBManage pb_manage_2(pb_dir_2, pb_file_name_2);
  ret = pb_manage_2.Init(pb_type_names_2);
  EXPECT_EQ(kOk, ret);

  google::protobuf::Message *ref_animal = pb_manage_2.NewMsg("Animal");
  EXPECT_NEQ(NULL, ref_animal);
  ret = proto::JsonToPBWithOutExtension(source_json_2, ref_animal);
  EXPECT_EQ(kOk, ret);
  fprintf(stderr, "animal:%s\n", ref_animal->DebugString().c_str());
  delete ref_animal;

  // NOTE: check whether pb_manage is ok?
  ref_person = pb_manage.NewMsg("Person");
  EXPECT_NEQ(NULL, ref_person);

  EXPECT_EQ(true, ref_person->ParseFromString(source_person_str));
  fprintf(stderr, "person:%s\n", ref_person->DebugString().c_str());
  EXPECT_EQ(person.DebugString(), ref_person->DebugString());
  delete ref_person;
} /*}}}*/

TEST(PBMange, Test_Exception_InvalidInitTypeName) { /*{{{*/
  using namespace base;

  std::string source_json =
      "{\"name\":\"zhangsan\",\"birthday\":1,\"is_student\":false,\"resv1\":\"good "
      "one\",\"resv2\":1234605616436508552,\"resv3\":-139292509886650761,\"health_status\":\"HS_"
      "MID\"}";

  model::Person person;
  Code ret = proto::JsonToPBWithOutExtension(source_json, &person);
  EXPECT_EQ(kOk, ret);

  // fprintf(stderr, "pb_str:\n%s\n", person.DebugString().c_str());
  // fprintf(stderr, "size:%zu, json:       %s\n", source_json.size(), source_json.c_str());

  std::string source_person_str;
  EXPECT_EQ(true, person.SerializeToString(&source_person_str));

  // NOTE: read from pb file;
  std::string pb_dir = "../protobuf";
  std::string pb_file_name = "model.proto";
  std::vector<std::string> pb_type_names;
  pb_type_names.push_back("NONOPerson");

  proto::PBManage pb_manage(pb_dir, pb_file_name);
  ret = pb_manage.Init(pb_type_names);
  EXPECT_EQ(kInvalidParam, ret);
} /*}}}*/

TEST(PBMange, Test_Exception_InvalidFindTypeName) { /*{{{*/
  using namespace base;

  std::string source_json =
      "{\"name\":\"zhangsan\",\"birthday\":1,\"is_student\":false,\"resv1\":\"good "
      "one\",\"resv2\":1234605616436508552,\"resv3\":-139292509886650761,\"health_status\":\"HS_"
      "MID\"}";

  model::Person person;
  Code ret = proto::JsonToPBWithOutExtension(source_json, &person);
  EXPECT_EQ(kOk, ret);

  // fprintf(stderr, "pb_str:\n%s\n", person.DebugString().c_str());
  // fprintf(stderr, "size:%zu, json:       %s\n", source_json.size(), source_json.c_str());

  std::string source_person_str;
  EXPECT_EQ(true, person.SerializeToString(&source_person_str));

  // NOTE: read from pb file;
  std::string pb_dir = "../protobuf";
  std::string pb_file_name = "model.proto";
  std::vector<std::string> pb_type_names;
  pb_type_names.push_back("Person");

  proto::PBManage pb_manage(pb_dir, pb_file_name);
  ret = pb_manage.Init(pb_type_names);
  EXPECT_EQ(kOk, ret);

  google::protobuf::Message *ref_person = pb_manage.NewMsg("NoPerson");
  EXPECT_EQ(NULL, ref_person);
} /*}}}*/

TEST(PBMange, Test_Exception_InvalidFileName) { /*{{{*/
  using namespace base;

  std::string source_json =
      "{\"name\":\"zhangsan\",\"birthday\":1,\"is_student\":false,\"resv1\":\"good "
      "one\",\"resv2\":1234605616436508552,\"resv3\":-139292509886650761,\"health_status\":\"HS_"
      "MID\"}";

  model::Person person;
  Code ret = proto::JsonToPBWithOutExtension(source_json, &person);
  EXPECT_EQ(kOk, ret);

  // fprintf(stderr, "pb_str:\n%s\n", person.DebugString().c_str());
  // fprintf(stderr, "size:%zu, json:       %s\n", source_json.size(), source_json.c_str());

  std::string source_person_str;
  EXPECT_EQ(true, person.SerializeToString(&source_person_str));

  // NOTE: read from pb file;
  std::string pb_dir = "../protobuf";
  std::string pb_file_name = "no_exist_model.proto";
  std::vector<std::string> pb_type_names;
  pb_type_names.push_back("Person");

  proto::PBManage pb_manage(pb_dir, pb_file_name);
  ret = pb_manage.Init(pb_type_names);
  EXPECT_EQ(kImportFailed, ret);
} /*}}}*/

TEST(ProtoCheck, Test_Normal_LittleParamToBigParam) { /*{{{*/
  using namespace base;

  model::LittleParam little_param;
  uint32_t src_num = (uint64_t)(1 << 32) - 1;
  little_param.set_num(src_num);
  std::string tmp;
  EXPECT_EQ(true, little_param.SerializeToString(&tmp));

  model::BigParam big_param;
  EXPECT_EQ(true, big_param.ParseFromString(tmp));
  EXPECT_EQ(src_num, big_param.num());

  fprintf(stderr, "src_num:%u, little_param:%u, big_param:%ld\n", src_num, little_param.num(), big_param.num());
} /*}}}*/

TEST(ProtoCheck, Test_Normal_LittleParamToBigParam_Array) { /*{{{*/
  using namespace base;

  uint32_t max_uint32_num = (uint64_t)(1 << 32) - 1;
  uint32_t src_num[] = {0, 10, 100, 1 << 10, 1 << 20, max_uint32_num};
  for (int i = 0; i < sizeof(src_num) / sizeof(src_num[0]); ++i) {
    model::LittleParam little_param;
    little_param.set_num(src_num[i]);
    std::string tmp;
    EXPECT_EQ(true, little_param.SerializeToString(&tmp));

    model::BigParam big_param;
    EXPECT_EQ(true, big_param.ParseFromString(tmp));
    EXPECT_EQ(src_num[i], big_param.num());
    EXPECT_EQ(little_param.num(), big_param.num());

    fprintf(stderr, "src_num:%u, little_param:%u, big_param:%ld\n", src_num[i], little_param.num(), big_param.num());
  }
} /*}}}*/

TEST(ProtoCheck, Test_Normal_BigParamToLittleParam_Array) { /*{{{*/
  using namespace base;

  uint32_t max_uint32_num = (uint64_t)(1 << 32) - 1;
  uint32_t src_num[] = {0, 10, 100, 1 << 10, 1 << 20, max_uint32_num};
  for (int i = 0; i < sizeof(src_num) / sizeof(src_num[0]); ++i) {
    model::BigParam big_param;
    big_param.set_num(src_num[i]);
    std::string tmp;
    EXPECT_EQ(true, big_param.SerializeToString(&tmp));

    model::LittleParam little_param;
    EXPECT_EQ(true, little_param.ParseFromString(tmp));
    EXPECT_EQ(src_num[i], little_param.num());
    EXPECT_EQ(big_param.num(), little_param.num());

    fprintf(stderr, "src_num:%u, little_param:%u, big_param:%ld\n", src_num[i], little_param.num(), big_param.num());
  }
} /*}}}*/

TEST(ProtoCheck, Test_Exception_BigParamToLittleParam) { /*{{{*/
  using namespace base;

  model::BigParam big_param;
  int64_t src_num = 0x1a2bffffffff - 1;
  big_param.set_num(src_num);
  std::string tmp;
  EXPECT_EQ(true, big_param.SerializeToString(&tmp));

  model::LittleParam little_param;
  EXPECT_EQ(true, little_param.ParseFromString(tmp));
  EXPECT_NEQ(src_num, little_param.num());

  fprintf(stderr, "src_num:%#lx,%ld, little_param:%lu, big_param:%ld\n", src_num, src_num, little_param.num(),
          big_param.num());
} /*}}}*/

TEST(ProtoCheck, Test_Normal_MoreParamToLessParam) { /*{{{*/
  using namespace base;

  model::MoreParams more_param;
  more_param.set_age(10);
  more_param.set_name("zhansan");
  more_param.set_fee(11);
  more_param.set_height(12);
  more_param.set_like(13);

  std::string tmp;
  EXPECT_EQ(true, more_param.SerializeToString(&tmp));

  model::LessParams less_param;
  EXPECT_EQ(true, less_param.ParseFromString(tmp));

  fprintf(stderr, "more_param:%s, less_param:%s\n", more_param.DebugString().c_str(), less_param.DebugString().c_str());

  std::string json;
  Code ret = proto::PBToJsonWithOutExtension(less_param, &json);
  EXPECT_EQ(kOk, ret);
  fprintf(stderr, "json:%s\n", json.c_str());
} /*}}}*/

TEST(ProtoCheck, Test_Exception_LittleParamToErrorParam) { /*{{{*/
  using namespace base;

  model::LittleParam little_param;
  uint32_t src_num = (uint64_t)(1 << 32) - 1;
  little_param.set_num(src_num);
  std::string tmp;
  EXPECT_EQ(true, little_param.SerializeToString(&tmp));

  model::ErrorParam err_param;
  EXPECT_EQ(true, err_param.ParseFromString(tmp));

  fprintf(stderr, "src_num:%u, little_param:%s, err_param:%s\n", src_num, little_param.DebugString().c_str(),
          err_param.DebugString().c_str());
} /*}}}*/

TEST(PBToJsonWithOutExtension, Test_Press_PB_Parse_Person) { /*{{{*/
  using namespace base;

  model::Person person;
  person.set_name("zhangsan");
  person.set_age(100);
  person.set_country_name("China");
  person.set_birthday(1);
  person.set_height(1.73);
  person.set_weight(105.413);
  person.set_is_student(false);
  person.set_resv1("good one");
  person.set_resv2(0x1122334455667788);
  person.set_resv3(0xfe11223344556677);

  std::string tmp;
  EXPECT_EQ(true, person.SerializeToString(&tmp));

  for (int i = 0; i < 100000; ++i) {
    model::Person tmp_person;
    EXPECT_EQ(true, tmp_person.ParseFromString(tmp));
  }
  model::Person tmp_person;
  EXPECT_EQ(true, tmp_person.ParseFromString(tmp));
  EXPECT_EQ(person.DebugString(), tmp_person.DebugString());
} /*}}}*/

TEST_D(PBToJsonWithOutExtension, Test_Press_JSON_Parse_Person, "压测JSON不校验UTF8性能") { /*{{{*/
  using namespace base;

  model::Person person;
  person.set_name("zhangsan");
  person.set_age(100);
  person.set_country_name("China");
  person.set_birthday(1);
  person.set_height(1.73);
  person.set_weight(105.413);
  person.set_is_student(false);
  person.set_resv1("good one");
  person.set_resv2(0x1122334455667788);
  person.set_resv3(0xfe11223344556677);

  std::string json;
  Code ret = proto::PBToJsonWithOutExtension(person, &json);
  EXPECT_EQ(kOk, ret);

  std::string expect_json =
      "{\"name\":\"zhangsan\",\"age\":100,\"country_name\":\"China\",\"birthday\":1,\"height\":1."
      "73,\"weight\":105.41300201416016,\"is_student\":false,\"resv1\":\"good "
      "one\",\"resv2\":1234605616436508552,\"resv3\":-139292509886650761}";
  EXPECT_EQ(json, expect_json);

  for (int i = 0; i < 100000; ++i) {
    rapidjson::Document d;
    d.Parse(json.c_str(), json.size());
    EXPECT_EQ(false, d.HasParseError());
  }

  rapidjson::Document d;
  d.Parse(json.c_str(), json.size());
  EXPECT_EQ(false, d.HasParseError());

  rapidjson::StringBuffer buffer;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
  d.Accept(writer);
  std::string dst_json(buffer.GetString(), buffer.GetSize());
  EXPECT_EQ(dst_json, expect_json);
  fprintf(stderr, "dst_json:%s\n", dst_json.c_str());
} /*}}}*/

TEST_D(PBToJsonWithOutExtension, Test_Press_JSON_Parse_Person_CheckUTF8, "压测JSON校验UTF8性能") { /*{{{*/
  using namespace base;

  model::Person person;
  person.set_name("zhangsan");
  person.set_age(100);
  person.set_country_name("China");
  person.set_birthday(1);
  person.set_height(1.73);
  person.set_weight(105.413);
  person.set_is_student(false);
  person.set_resv1("good one");
  person.set_resv2(0x1122334455667788);
  person.set_resv3(0xfe11223344556677);

  std::string json;
  Code ret = proto::PBToJsonWithOutExtension(person, &json);
  EXPECT_EQ(kOk, ret);

  std::string expect_json =
      "{\"name\":\"zhangsan\",\"age\":100,\"country_name\":\"China\",\"birthday\":1,\"height\":1."
      "73,\"weight\":105.41300201416016,\"is_student\":false,\"resv1\":\"good "
      "one\",\"resv2\":1234605616436508552,\"resv3\":-139292509886650761}";
  EXPECT_EQ(json, expect_json);

  for (int i = 0; i < 100000; ++i) {
    rapidjson::Document d;
    d.Parse<rapidjson::kParseValidateEncodingFlag>(json.c_str(), json.size());
    EXPECT_EQ(false, d.HasParseError());
  }

  rapidjson::Document d;
  d.Parse(json.c_str(), json.size());
  EXPECT_EQ(false, d.HasParseError());

  rapidjson::StringBuffer buffer;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
  d.Accept(writer);
  std::string dst_json(buffer.GetString(), buffer.GetSize());
  EXPECT_EQ(dst_json, expect_json);
  fprintf(stderr, "dst_json:%s\n", dst_json.c_str());
} /*}}}*/

TEST(ProtoCheck, Test_Normal_MoreParamToLessParam_Null) { /*{{{*/
  using namespace base;

  model::MoreParams more_param;
  std::string str;
  //    str.assign(NULL); // NOTE: empe, pb not allow to be assigned to null

  fprintf(stderr, "more_param:%d\n", more_param.has_name());

} /*}}}*/

TEST(PBDiffWithOutExtension, Test_Normal_SameType) { /*{{{*/
  using namespace base;
  using namespace proto;

  model::Person person1;
  { /*{{{*/
    person1.set_name("zhangsan");
    person1.set_age(100);
    person1.set_country_name("China");
    person1.set_birthday(1);
    person1.set_height(1.7312389233245);
    person1.set_weight(105.413);
    person1.set_is_student(false);
    person1.set_resv1("good one");
    person1.set_resv2(0x1122334455667788);
    person1.set_resv3(0xfe11223344556677);
    person1.set_health_status(model::HS_GOOD);
    model::Addr *addr = person1.mutable_addrs();
    addr->set_place("New York");
    addr->set_num(1);
    model::OpenAI *openai = person1.mutable_openai();
    openai->set_name("lisi hello one");
    openai->set_ai1(0x7fffffff);
    openai->set_ai2(0xffffffff);
    openai->set_ai3(0x7fffffffffffffff);
    openai->set_ai4(0xffffffffffffffff);
    openai->set_ai5(1231.123123123123);
    openai->set_ai6(89234.8912123);
    openai->set_ai7(true);
    openai->set_ai8(model::HS_MID);
    openai->set_ai9("test for good and you may go to this, could you give me some advise");
    model::ChatGPT *chat_gpt = openai->mutable_chat_gpt();
    chat_gpt->set_name("lisi hello two");
    chat_gpt->set_gpt1(0x7fffffff);
    chat_gpt->set_gpt2(0xffffffff);
    chat_gpt->set_gpt3(0x7fffffffffffffff);
    chat_gpt->set_gpt4(0xffffffffffffffff);
    chat_gpt->set_gpt5(1231.123123123123);
    chat_gpt->set_gpt6(89234.8912123);
    chat_gpt->set_gpt7(true);
    chat_gpt->set_gpt8(model::HS_MID);
    chat_gpt->set_gpt9("test for good and you may go to this, could you give me some advise");
  } /*}}}*/

  model::Person person2 = person1;

  bool is_diff = false;
  std::deque<DiffContent> diff_contents;
  Code ret = proto::PBDiffWithOutExtension(person1, person2, &is_diff, &diff_contents);
  EXPECT_EQ(kOk, ret);
  EXPECT_EQ(false, is_diff);

  // just check whether is different
  ret = proto::PBDiffWithOutExtension(person1, person2, &is_diff, NULL);
  EXPECT_EQ(kOk, ret);
  EXPECT_EQ(false, is_diff);

  // fprintf(stderr, "person1:\n%s\n", person1.DebugString().c_str());
  // fprintf(stderr, "person2:\n%s\n", person2.DebugString().c_str());
  // fprintf(stderr, "size:%zu, json:       %s\n", json.size(), json.c_str());
  // fprintf(stderr, "size:%zu, expect_json:%s\n", expect_json.size(), expect_json.c_str());
} /*}}}*/

TEST(PBDiffWithOutExtension, Test_Normal_Empty_SameType) { /*{{{*/
  using namespace base;
  using namespace proto;

  model::Person person1;
  person1.set_name("aabb");
  model::Person person2 = person1;

  bool is_diff = false;
  std::deque<DiffContent> diff_contents;
  Code ret = proto::PBDiffWithOutExtension(person1, person2, &is_diff, &diff_contents);
  EXPECT_EQ(kOk, ret);
  EXPECT_EQ(false, is_diff);

  // just check whether is different
  ret = proto::PBDiffWithOutExtension(person1, person2, &is_diff, NULL);
  EXPECT_EQ(kOk, ret);
  EXPECT_EQ(false, is_diff);

  // fprintf(stderr, "person1:\n%s\n", person1.DebugString().c_str());
  // fprintf(stderr, "person2:\n%s\n", person2.DebugString().c_str());
  // fprintf(stderr, "size:%zu, json:       %s\n", json.size(), json.c_str());
  // fprintf(stderr, "size:%zu, expect_json:%s\n", expect_json.size(), expect_json.c_str());
} /*}}}*/

TEST(PBDiffWithOutExtension, Test_Normal_DiffType_ValueNotEqual) { /*{{{*/
  using namespace base;
  using namespace proto;

  model::Person person1;
  { /*{{{*/
    person1.set_name("zhangsan");
    person1.set_age(100);
    person1.set_country_name("China");
    person1.set_birthday(1);
    person1.set_height(1.7312389233245);
    person1.set_weight(105.413);
    person1.set_is_student(false);
    person1.set_resv1("good one");
    person1.set_resv2(0x1122334455667788);
    person1.set_resv3(0xfe11223344556677);
    person1.set_health_status(model::HS_GOOD);
    model::Addr *addr = person1.mutable_addrs();
    addr->set_place("New York");
    addr->set_num(1);
    model::OpenAI *openai = person1.mutable_openai();
    openai->set_name("lisi hello one");
    openai->set_ai1(0x7fffffff);
    openai->set_ai2(0xffffffff);
    openai->set_ai3(0x7fffffffffffffff);
    openai->set_ai4(0xffffffffffffffff);
    openai->set_ai5(1231.123123123123);
    openai->set_ai6(89234.8912123);
    openai->set_ai7(true);
    openai->set_ai8(model::HS_MID);
    openai->set_ai9("test for good and you may go to this, could you give me some advise");
    model::ChatGPT *chat_gpt = openai->mutable_chat_gpt();
    chat_gpt->set_name("gpt1");
    chat_gpt->set_gpt1(0x7fffffff);
    chat_gpt->set_gpt2(0xffffffff);
    chat_gpt->set_gpt3(0x7fffffffffffffff);
    chat_gpt->set_gpt4(0xffffffffffffffff);
    chat_gpt->set_gpt5(12.123113);
    chat_gpt->set_gpt6(89.8923);
    chat_gpt->set_gpt7(true);
    chat_gpt->set_gpt8(model::HS_MID);
    chat_gpt->set_gpt9("test for gpt1");
  } /*}}}*/

  model::Person person2 = person1;
  { /*{{{*/
    model::ChatGPT *chat_gpt = person2.mutable_openai()->mutable_chat_gpt();
    chat_gpt->set_name("gpt2");
    chat_gpt->set_gpt1(0x7ffffffe);
    chat_gpt->set_gpt2(0xfffffffe);
    chat_gpt->set_gpt3(0x7ffffffffffffffe);
    chat_gpt->set_gpt4(0xfffffffffffffffe);
    chat_gpt->set_gpt5(12.123114);
    chat_gpt->set_gpt6(89.8924);
    chat_gpt->set_gpt7(false);
    chat_gpt->set_gpt8(model::HS_BAD);
    chat_gpt->set_gpt9("test for gpt2");
  } /*}}}*/

  std::deque<DiffContent> dst_diff_contents;
  { /*{{{*/
    DiffContent diff_content0;
    diff_content0.status = kValueNotEqual;
    diff_content0.key = "openai.chat_gpt.name";
    diff_content0.type = kString;
    diff_content0.value.first_value_string = "gpt1";
    diff_content0.value.second_value_string = "gpt2";
    dst_diff_contents.push_back(diff_content0);
  } /*}}}*/
  { /*{{{*/
    DiffContent diff_content1;
    diff_content1.status = kValueNotEqual;
    diff_content1.key = "openai.chat_gpt.gpt1";
    diff_content1.type = kInt32;
    diff_content1.value.first_value_int32 = 0x7fffffff;
    diff_content1.value.second_value_int32 = 0x7ffffffe;
    dst_diff_contents.push_back(diff_content1);
  } /*}}}*/
  { /*{{{*/
    DiffContent diff_content2;
    diff_content2.status = kValueNotEqual;
    diff_content2.key = "openai.chat_gpt.gpt2";
    diff_content2.type = kUInt32;
    diff_content2.value.first_value_uint32 = 0xffffffff;
    diff_content2.value.second_value_uint32 = 0xfffffffe;
    dst_diff_contents.push_back(diff_content2);
  } /*}}}*/
  { /*{{{*/
    DiffContent diff_content3;
    diff_content3.status = kValueNotEqual;
    diff_content3.key = "openai.chat_gpt.gpt3";
    diff_content3.type = kInt64;
    diff_content3.value.first_value_int64 = 0x7fffffffffffffff;
    diff_content3.value.second_value_int64 = 0x7ffffffffffffffe;
    dst_diff_contents.push_back(diff_content3);
  } /*}}}*/
  { /*{{{*/
    DiffContent diff_content4;
    diff_content4.status = kValueNotEqual;
    diff_content4.key = "openai.chat_gpt.gpt4";
    diff_content4.type = kUInt64;
    diff_content4.value.first_value_uint64 = 0xffffffffffffffff;
    diff_content4.value.second_value_uint64 = 0xfffffffffffffffe;
    dst_diff_contents.push_back(diff_content4);
  } /*}}}*/
  { /*{{{*/
    DiffContent diff_content5;
    diff_content5.status = kValueNotEqual;
    diff_content5.key = "openai.chat_gpt.gpt5";
    diff_content5.type = kDouble;
    diff_content5.value.first_value_double = 12.123113;
    diff_content5.value.second_value_double = 12.123114;
    dst_diff_contents.push_back(diff_content5);
  } /*}}}*/
  { /*{{{*/
    DiffContent diff_content6;
    diff_content6.status = kValueNotEqual;
    diff_content6.key = "openai.chat_gpt.gpt6";
    diff_content6.type = kFloat;
    diff_content6.value.first_value_float = 89.8923;
    diff_content6.value.second_value_float = 89.8924;
    dst_diff_contents.push_back(diff_content6);
  } /*}}}*/
  { /*{{{*/
    DiffContent diff_content7;
    diff_content7.status = kValueNotEqual;
    diff_content7.key = "openai.chat_gpt.gpt7";
    diff_content7.type = kBool;
    diff_content7.value.first_value_bool = true;
    diff_content7.value.second_value_bool = false;
    dst_diff_contents.push_back(diff_content7);
  } /*}}}*/
  { /*{{{*/
    DiffContent diff_content8;
    diff_content8.status = kValueNotEqual;
    diff_content8.key = "openai.chat_gpt.gpt8";
    diff_content8.type = kEnum;
    diff_content8.value.first_value_enum = 102;
    diff_content8.value.second_value_enum = 103;
    dst_diff_contents.push_back(diff_content8);
  } /*}}}*/
  { /*{{{*/
    DiffContent diff_content9;
    diff_content9.status = kValueNotEqual;
    diff_content9.key = "openai.chat_gpt.gpt9";
    diff_content9.type = kString;
    diff_content9.value.first_value_string = "test for gpt1";
    diff_content9.value.second_value_string = "test for gpt2";
    dst_diff_contents.push_back(diff_content9);
  } /*}}}*/

  bool is_diff = false;
  std::deque<DiffContent> diff_contents;
  Code ret = proto::PBDiffWithOutExtension(person1, person2, &is_diff, &diff_contents);
  EXPECT_EQ(kOk, ret);
  EXPECT_EQ(true, is_diff);
  fprintf(stderr, "diff size:%zu\n", diff_contents.size());
  EXPECT_EQ(10, diff_contents.size());
  EXPECT_EQ(dst_diff_contents.size(), diff_contents.size());

  for (size_t i = 0; i < diff_contents.size(); ++i) {
    diff_contents[i].Print();
    fprintf(stderr, "\n");
    EXPECT_EQ(true, (diff_contents[i] == dst_diff_contents[i]));
  }

  // just check whether is different
  ret = proto::PBDiffWithOutExtension(person1, person2, &is_diff, NULL);
  EXPECT_EQ(kOk, ret);
  EXPECT_EQ(true, is_diff);

  //    fprintf(stderr, "person1:\n%s\n", person1.DebugString().c_str());
  //    fprintf(stderr, "person2:\n%s\n", person2.DebugString().c_str());
} /*}}}*/

TEST(PBDiffWithOutExtension, Test_Normal_DiffType_SecondEmptyType) { /*{{{*/
  using namespace base;
  using namespace proto;

  model::Person person1;
  { /*{{{*/
    person1.set_name("zhangsan");
    person1.set_age(100);
    person1.set_country_name("China");
    person1.set_birthday(1);
    person1.set_height(1.7312389233245);
    person1.set_weight(105.413);
    person1.set_is_student(false);
    person1.set_resv1("good one");
    person1.set_resv2(0x1122334455667788);
    person1.set_resv3(0xfe11223344556677);
    person1.set_health_status(model::HS_GOOD);
    model::Addr *addr = person1.mutable_addrs();
    addr->set_place("New York");
    addr->set_num(1);
    model::OpenAI *openai = person1.mutable_openai();
    openai->set_name("lisi hello one");
    openai->set_ai1(0x7fffffff);
    openai->set_ai2(0xffffffff);
    openai->set_ai3(0x7fffffffffffffff);
    openai->set_ai4(0xffffffffffffffff);
    openai->set_ai5(1231.123123123123);
    openai->set_ai6(89234.8912123);
    openai->set_ai7(true);
    openai->set_ai8(model::HS_MID);
    openai->set_ai9("test for good and you may go to this, could you give me some advise");
    model::ChatGPT *chat_gpt = openai->mutable_chat_gpt();
    chat_gpt->set_name("gpt1");
    chat_gpt->set_gpt1(0x7fffffff);
    chat_gpt->set_gpt2(0xffffffff);
    chat_gpt->set_gpt3(0x7fffffffffffffff);
    chat_gpt->set_gpt4(0xffffffffffffffff);
    chat_gpt->set_gpt5(12.123113);
    chat_gpt->set_gpt6(89.8923);
    chat_gpt->set_gpt7(true);
    chat_gpt->set_gpt8(model::HS_MID);
    chat_gpt->set_gpt9("test for gpt1");
  } /*}}}*/

  model::Person person2 = person1;
  { /*{{{*/
    person2.clear_addrs();
    model::ChatGPT *chat_gpt = person2.mutable_openai()->mutable_chat_gpt();
    chat_gpt->set_name("gpt2");
    chat_gpt->clear_gpt1();
    chat_gpt->clear_gpt2();
    chat_gpt->clear_gpt3();
    chat_gpt->clear_gpt4();
    chat_gpt->clear_gpt5();
    chat_gpt->clear_gpt6();
    chat_gpt->clear_gpt7();
    chat_gpt->clear_gpt8();
    chat_gpt->clear_gpt9();
  } /*}}}*/

  std::deque<DiffContent> dst_diff_contents;
  { /*{{{*/
    DiffContent diff_content00;
    diff_content00.status = kSecondValueEmpty;
    diff_content00.key = "addrs";
    diff_content00.type = kObject;
    dst_diff_contents.push_back(diff_content00);
  } /*}}}*/
  { /*{{{*/
    DiffContent diff_content0;
    diff_content0.status = kValueNotEqual;
    diff_content0.key = "openai.chat_gpt.name";
    diff_content0.type = kString;
    diff_content0.value.first_value_string = "gpt1";
    diff_content0.value.second_value_string = "gpt2";
    dst_diff_contents.push_back(diff_content0);
  } /*}}}*/
  { /*{{{*/
    DiffContent diff_content1;
    diff_content1.status = kSecondValueEmpty;
    diff_content1.key = "openai.chat_gpt.gpt1";
    diff_content1.type = kInt32;
    diff_content1.value.first_value_int32 = 0x7fffffff;
    dst_diff_contents.push_back(diff_content1);
  } /*}}}*/
  { /*{{{*/
    DiffContent diff_content2;
    diff_content2.status = kSecondValueEmpty;
    diff_content2.key = "openai.chat_gpt.gpt2";
    diff_content2.type = kUInt32;
    diff_content2.value.first_value_uint32 = 0xffffffff;
    dst_diff_contents.push_back(diff_content2);
  } /*}}}*/
  { /*{{{*/
    DiffContent diff_content3;
    diff_content3.status = kSecondValueEmpty;
    diff_content3.key = "openai.chat_gpt.gpt3";
    diff_content3.type = kInt64;
    diff_content3.value.first_value_int64 = 0x7fffffffffffffff;
    dst_diff_contents.push_back(diff_content3);
  } /*}}}*/
  { /*{{{*/
    DiffContent diff_content4;
    diff_content4.status = kSecondValueEmpty;
    diff_content4.key = "openai.chat_gpt.gpt4";
    diff_content4.type = kUInt64;
    diff_content4.value.first_value_uint64 = 0xffffffffffffffff;
    dst_diff_contents.push_back(diff_content4);
  } /*}}}*/
  { /*{{{*/
    DiffContent diff_content5;
    diff_content5.status = kSecondValueEmpty;
    diff_content5.key = "openai.chat_gpt.gpt5";
    diff_content5.type = kDouble;
    diff_content5.value.first_value_double = 12.123113;
    dst_diff_contents.push_back(diff_content5);
  } /*}}}*/
  { /*{{{*/
    DiffContent diff_content6;
    diff_content6.status = kSecondValueEmpty;
    diff_content6.key = "openai.chat_gpt.gpt6";
    diff_content6.type = kFloat;
    diff_content6.value.first_value_float = 89.8923;
    dst_diff_contents.push_back(diff_content6);
  } /*}}}*/
  { /*{{{*/
    DiffContent diff_content7;
    diff_content7.status = kSecondValueEmpty;
    diff_content7.key = "openai.chat_gpt.gpt7";
    diff_content7.type = kBool;
    diff_content7.value.first_value_bool = true;
    dst_diff_contents.push_back(diff_content7);
  } /*}}}*/
  { /*{{{*/
    DiffContent diff_content8;
    diff_content8.status = kSecondValueEmpty;
    diff_content8.key = "openai.chat_gpt.gpt8";
    diff_content8.type = kEnum;
    diff_content8.value.first_value_enum = 102;
    dst_diff_contents.push_back(diff_content8);
  } /*}}}*/
  { /*{{{*/
    DiffContent diff_content9;
    diff_content9.status = kSecondValueEmpty;
    diff_content9.key = "openai.chat_gpt.gpt9";
    diff_content9.type = kString;
    diff_content9.value.first_value_string = "test for gpt1";
    dst_diff_contents.push_back(diff_content9);
  } /*}}}*/

  bool is_diff = false;
  std::deque<DiffContent> diff_contents;
  Code ret = proto::PBDiffWithOutExtension(person1, person2, &is_diff, &diff_contents);
  EXPECT_EQ(kOk, ret);
  EXPECT_EQ(true, is_diff);
  fprintf(stderr, "diff size:%zu\n", diff_contents.size());
  EXPECT_EQ(11, diff_contents.size());
  EXPECT_EQ(dst_diff_contents.size(), diff_contents.size());

  for (size_t i = 0; i < diff_contents.size(); ++i) {
    diff_contents[i].Print();
    fprintf(stderr, "\n");
    EXPECT_EQ(true, (diff_contents[i] == dst_diff_contents[i]));
  }

  // just check whether is different
  ret = proto::PBDiffWithOutExtension(person1, person2, &is_diff, NULL);
  EXPECT_EQ(kOk, ret);
  EXPECT_EQ(true, is_diff);

  //    fprintf(stderr, "person1:\n%s\n", person1.DebugString().c_str());
  //    fprintf(stderr, "person2:\n%s\n", person2.DebugString().c_str());
} /*}}}*/

TEST(PBDiffWithOutExtension, Test_Normal_DiffType_FirstEmptyType) { /*{{{*/
  using namespace base;
  using namespace proto;

  model::Person person1;
  { /*{{{*/
    person1.set_name("zhangsan");
    person1.set_age(100);
    person1.set_country_name("China");
    person1.set_birthday(1);
    person1.set_height(1.7312389233245);
    person1.set_weight(105.413);
    person1.set_is_student(false);
    person1.set_resv1("good one");
    person1.set_resv2(0x1122334455667788);
    person1.set_resv3(0xfe11223344556677);
    person1.set_health_status(model::HS_GOOD);
    model::OpenAI *openai = person1.mutable_openai();
    openai->set_name("lisi hello one");
    openai->set_ai1(0x7fffffff);
    openai->set_ai2(0xffffffff);
    openai->set_ai3(0x7fffffffffffffff);
    openai->set_ai4(0xffffffffffffffff);
    openai->set_ai5(1231.123123123123);
    openai->set_ai6(89234.8912123);
    openai->set_ai7(true);
    openai->set_ai8(model::HS_MID);
    openai->set_ai9("test for good and you may go to this, could you give me some advise");
    model::ChatGPT *chat_gpt = openai->mutable_chat_gpt();
    chat_gpt->set_name("gpt1");
  } /*}}}*/

  model::Person person2 = person1;
  { /*{{{*/
    model::Addr *addr = person2.mutable_addrs();
    addr->set_place("New York");
    addr->set_num(1);

    model::ChatGPT *chat_gpt = person2.mutable_openai()->mutable_chat_gpt();
    chat_gpt->set_name("gpt2");
    chat_gpt->set_gpt1(0x7ffffffe);
    chat_gpt->set_gpt2(0xfffffffe);
    chat_gpt->set_gpt3(0x7ffffffffffffffe);
    chat_gpt->set_gpt4(0xfffffffffffffffe);
    chat_gpt->set_gpt5(12.123114);
    chat_gpt->set_gpt6(89.8924);
    chat_gpt->set_gpt7(false);
    chat_gpt->set_gpt8(model::HS_BAD);
    chat_gpt->set_gpt9("test for gpt2");
  } /*}}}*/

  std::deque<DiffContent> dst_diff_contents;
  { /*{{{*/
    DiffContent diff_content00;
    diff_content00.status = kFirstValueEmpty;
    diff_content00.key = "addrs";
    diff_content00.type = kObject;
    dst_diff_contents.push_back(diff_content00);
  } /*}}}*/
  { /*{{{*/
    DiffContent diff_content0;
    diff_content0.status = kValueNotEqual;
    diff_content0.key = "openai.chat_gpt.name";
    diff_content0.type = kString;
    diff_content0.value.first_value_string = "gpt1";
    diff_content0.value.second_value_string = "gpt2";
    dst_diff_contents.push_back(diff_content0);
  } /*}}}*/
  { /*{{{*/
    DiffContent diff_content1;
    diff_content1.status = kFirstValueEmpty;
    diff_content1.key = "openai.chat_gpt.gpt1";
    diff_content1.type = kInt32;
    diff_content1.value.second_value_int32 = 0x7ffffffe;
    dst_diff_contents.push_back(diff_content1);
  } /*}}}*/
  { /*{{{*/
    DiffContent diff_content2;
    diff_content2.status = kFirstValueEmpty;
    diff_content2.key = "openai.chat_gpt.gpt2";
    diff_content2.type = kUInt32;
    diff_content2.value.second_value_uint32 = 0xfffffffe;
    dst_diff_contents.push_back(diff_content2);
  } /*}}}*/
  { /*{{{*/
    DiffContent diff_content3;
    diff_content3.status = kFirstValueEmpty;
    diff_content3.key = "openai.chat_gpt.gpt3";
    diff_content3.type = kInt64;
    diff_content3.value.second_value_int64 = 0x7ffffffffffffffe;
    dst_diff_contents.push_back(diff_content3);
  } /*}}}*/
  { /*{{{*/
    DiffContent diff_content4;
    diff_content4.status = kFirstValueEmpty;
    diff_content4.key = "openai.chat_gpt.gpt4";
    diff_content4.type = kUInt64;
    diff_content4.value.second_value_uint64 = 0xfffffffffffffffe;
    dst_diff_contents.push_back(diff_content4);
  } /*}}}*/
  { /*{{{*/
    DiffContent diff_content5;
    diff_content5.status = kFirstValueEmpty;
    diff_content5.key = "openai.chat_gpt.gpt5";
    diff_content5.type = kDouble;
    diff_content5.value.second_value_double = 12.123114;
    dst_diff_contents.push_back(diff_content5);
  } /*}}}*/
  { /*{{{*/
    DiffContent diff_content6;
    diff_content6.status = kFirstValueEmpty;
    diff_content6.key = "openai.chat_gpt.gpt6";
    diff_content6.type = kFloat;
    diff_content6.value.second_value_float = 89.8924;
    dst_diff_contents.push_back(diff_content6);
  } /*}}}*/
  { /*{{{*/
    DiffContent diff_content7;
    diff_content7.status = kFirstValueEmpty;
    diff_content7.key = "openai.chat_gpt.gpt7";
    diff_content7.type = kBool;
    diff_content7.value.second_value_bool = false;
    dst_diff_contents.push_back(diff_content7);
  } /*}}}*/
  { /*{{{*/
    DiffContent diff_content8;
    diff_content8.status = kFirstValueEmpty;
    diff_content8.key = "openai.chat_gpt.gpt8";
    diff_content8.type = kEnum;
    diff_content8.value.second_value_enum = 103;
    dst_diff_contents.push_back(diff_content8);
  } /*}}}*/
  { /*{{{*/
    DiffContent diff_content9;
    diff_content9.status = kFirstValueEmpty;
    diff_content9.key = "openai.chat_gpt.gpt9";
    diff_content9.type = kString;
    diff_content9.value.second_value_string = "test for gpt2";
    dst_diff_contents.push_back(diff_content9);
  } /*}}}*/

  bool is_diff = false;
  std::deque<DiffContent> diff_contents;
  Code ret = proto::PBDiffWithOutExtension(person1, person2, &is_diff, &diff_contents);
  EXPECT_EQ(kOk, ret);
  EXPECT_EQ(true, is_diff);
  fprintf(stderr, "diff size:%zu\n", diff_contents.size());
  EXPECT_EQ(11, diff_contents.size());
  EXPECT_EQ(dst_diff_contents.size(), diff_contents.size());

  for (size_t i = 0; i < diff_contents.size(); ++i) {
    diff_contents[i].Print();
    fprintf(stderr, "\n");
    EXPECT_EQ(true, (diff_contents[i] == dst_diff_contents[i]));
  }

  // just check whether is different
  ret = proto::PBDiffWithOutExtension(person1, person2, &is_diff, NULL);
  EXPECT_EQ(kOk, ret);
  EXPECT_EQ(true, is_diff);

  //    fprintf(stderr, "person1:\n%s\n", person1.DebugString().c_str());
  //    fprintf(stderr, "person2:\n%s\n", person2.DebugString().c_str());
} /*}}}*/

TEST(PBDiffWithOutExtension, Test_Normal_Array_SameType) { /*{{{*/
  using namespace base;
  using namespace proto;

  model::Person person1;
  { /*{{{*/
    person1.set_name("zhangsan");
    person1.set_age(100);
    person1.set_country_name("China");
    person1.set_birthday(1);
    person1.set_height(1.7312389233245);
    person1.set_weight(105.413);
    person1.set_is_student(false);
    person1.set_resv1("good one");
    person1.set_resv2(0x1122334455667788);
    person1.set_resv3(0xfe11223344556677);
    person1.set_health_status(model::HS_GOOD);
    model::Addr *addr = person1.mutable_addrs();
    addr->set_place("New York");
    addr->set_num(1);
    model::CoreOpenAI *core_openai = person1.mutable_core_openai();
    core_openai->set_name("lisi hello one");
    core_openai->add_ais1(0x77ffffff);
    core_openai->add_ais1(0x7fffffff);
    core_openai->add_ais2(0xf7ffffff);
    core_openai->add_ais2(0xffffffff);
    core_openai->add_ais3(0x7fffffffffffffff);
    core_openai->add_ais3(0x77ffffffffffffff);
    core_openai->add_ais4(0xf7ffffffffffffff);
    core_openai->add_ais4(0xffffffffffffffff);
    core_openai->add_ais5(12.1231231);
    core_openai->add_ais5(12.2231231);
    core_openai->add_ais6(89.1912);
    core_openai->add_ais6(89.8912);
    core_openai->add_ais7(true);
    core_openai->add_ais7(false);
    core_openai->add_ais8(model::HS_MID);
    core_openai->add_ais8(model::HS_BAD);
    core_openai->add_ais9("test for good ai 001");
    core_openai->add_ais9("test for good ai 002");
    model::ChatGPT *chat_gpt = core_openai->add_chat_gpts();
    chat_gpt->set_name("gpt001");
    chat_gpt->set_gpt1(0x7ffffff1);
    chat_gpt->set_gpt2(0xfffffff1);
    chat_gpt->set_gpt3(0x7ffffffffffffff1);
    chat_gpt->set_gpt4(0xfffffffffffffff1);
    chat_gpt->set_gpt5(11.123001);
    chat_gpt->set_gpt6(11.0001);
    chat_gpt->set_gpt7(true);
    chat_gpt->set_gpt8(model::HS_MID);
    chat_gpt->set_gpt9("test for good gpt 001");
    chat_gpt = core_openai->add_chat_gpts();
    chat_gpt->set_name("gpt002");
    chat_gpt->set_gpt1(0x7ffffff2);
    chat_gpt->set_gpt2(0xfffffff2);
    chat_gpt->set_gpt3(0x7ffffffffffffff2);
    chat_gpt->set_gpt4(0xfffffffffffffff2);
    chat_gpt->set_gpt5(11.123002);
    chat_gpt->set_gpt6(11.0002);
    chat_gpt->set_gpt7(false);
    chat_gpt->set_gpt8(model::HS_GOOD);
    chat_gpt->set_gpt9("test for good gpt 002");
  } /*}}}*/

  model::Person person2 = person1;

  bool is_diff = false;
  std::deque<DiffContent> diff_contents;
  Code ret = proto::PBDiffWithOutExtension(person1, person2, &is_diff, &diff_contents);
  EXPECT_EQ(kOk, ret);
  EXPECT_EQ(false, is_diff);

  // just check whether is different
  ret = proto::PBDiffWithOutExtension(person1, person2, &is_diff, NULL);
  EXPECT_EQ(kOk, ret);
  EXPECT_EQ(false, is_diff);
} /*}}}*/

TEST(PBDiffWithOutExtension, Test_Normal_Array_ValueNotEqual) { /*{{{*/
  using namespace base;
  using namespace proto;

  model::Person person1;
  { /*{{{*/
    person1.set_name("zhangsan");
    person1.set_age(100);
    person1.set_country_name("China");
    person1.set_birthday(1);
    person1.set_height(1.7312389233245);
    person1.set_weight(105.413);
    person1.set_is_student(false);
    person1.set_resv1("good one");
    person1.set_resv2(0x1122334455667788);
    person1.set_resv3(0xfe11223344556677);
    person1.set_health_status(model::HS_GOOD);
    model::Addr *addr = person1.mutable_addrs();
    addr->set_place("New York");
    addr->set_num(1);
    model::CoreOpenAI *core_openai = person1.mutable_core_openai();
    core_openai->set_name("openai001");
    core_openai->add_ais1(0x77ffffe1);
    core_openai->add_ais1(0x7fffffe1);
    core_openai->add_ais2(0xf7ffffe1);
    core_openai->add_ais2(0xffffffe1);
    core_openai->add_ais3(0x7fffffffffffffe1);
    core_openai->add_ais3(0x7fffffffffffffe2);
    core_openai->add_ais4(0xffffffffffffffe1);
    core_openai->add_ais4(0xffffffffffffffe2);
    core_openai->add_ais5(44.123001);
    core_openai->add_ais5(44.123002);
    core_openai->add_ais6(55.19001);
    core_openai->add_ais6(55.19002);
    core_openai->add_ais7(true);
    core_openai->add_ais7(false);
    core_openai->add_ais8(model::HS_MID);
    core_openai->add_ais8(model::HS_BAD);
    core_openai->add_ais9("test for good ai 001");
    core_openai->add_ais9("test for good ai 002");
    model::ChatGPT *chat_gpt = core_openai->add_chat_gpts();
    chat_gpt->set_name("gpt001");
    chat_gpt->set_gpt1(0x7fffffe1);
    chat_gpt->set_gpt2(0xffffffe1);
    chat_gpt->set_gpt3(0x7fffffffffffffe1);
    chat_gpt->set_gpt4(0xffffffffffffffe1);
    chat_gpt->set_gpt5(11.123001);
    chat_gpt->set_gpt6(11.0001);
    chat_gpt->set_gpt7(true);
    chat_gpt->set_gpt8(model::HS_MID);
    chat_gpt->set_gpt9("test for good gpt 001");
    chat_gpt = core_openai->add_chat_gpts();
    chat_gpt->set_name("gpt002");
    chat_gpt->set_gpt1(0x7fffffe2);
    chat_gpt->set_gpt2(0xffffffe2);
    chat_gpt->set_gpt3(0x7fffffffffffffe2);
    chat_gpt->set_gpt4(0xffffffffffffffe2);
    chat_gpt->set_gpt5(11.123002);
    chat_gpt->set_gpt6(11.0002);
    chat_gpt->set_gpt7(false);
    chat_gpt->set_gpt8(model::HS_GOOD);
    chat_gpt->set_gpt9("test for good gpt 002");
  } /*}}}*/

  model::Person person2 = person1;
  { /*{{{*/
    person2.clear_core_openai();
    model::CoreOpenAI *core_openai = person2.mutable_core_openai();
    core_openai->set_name("openai002");
    core_openai->add_ais1(0x77ffffe1);
    core_openai->add_ais1(0x7fffffff);
    core_openai->add_ais2(0xf7ffffe1);
    core_openai->add_ais2(0xffffffff);
    core_openai->add_ais3(0x7fffffffffffffff);
    core_openai->add_ais3(0x7fffffffffffffe2);
    core_openai->add_ais4(0xffffffffffffffff);
    core_openai->add_ais4(0xffffffffffffffe2);
    core_openai->add_ais5(44.123001);
    core_openai->add_ais5(44.123102);
    core_openai->add_ais6(55.19101);
    core_openai->add_ais6(55.19002);
    core_openai->add_ais7(true);
    core_openai->add_ais7(true);
    core_openai->add_ais8(model::HS_MID);
    core_openai->add_ais8(model::HS_GOOD);
    core_openai->add_ais9("test for good ai 101");
    core_openai->add_ais9("test for good ai 002");
    model::ChatGPT *chat_gpt = core_openai->add_chat_gpts();
    chat_gpt->set_name("gpt001");
    chat_gpt->set_gpt1(0x7fffffe1);
    chat_gpt->set_gpt2(0xffffffe1);
    chat_gpt->set_gpt3(0x7fffffffffffffe1);
    chat_gpt->set_gpt4(0xffffffffffffffe1);
    chat_gpt->set_gpt5(11.123001);
    chat_gpt->set_gpt6(11.0001);
    chat_gpt->set_gpt7(true);
    chat_gpt->set_gpt8(model::HS_MID);
    chat_gpt->set_gpt9("test for good gpt 001");
    chat_gpt = core_openai->add_chat_gpts();
    chat_gpt->set_name("gpt102");
    chat_gpt->set_gpt1(0x7fffffff);
    chat_gpt->set_gpt2(0xffffffff);
    chat_gpt->set_gpt3(0x7fffffffffffffff);
    chat_gpt->set_gpt4(0xffffffffffffffff);
    chat_gpt->set_gpt5(11.123102);
    chat_gpt->set_gpt6(11.0102);
    chat_gpt->set_gpt7(true);
    chat_gpt->set_gpt8(model::HS_MID);
    chat_gpt->set_gpt9("test for good gpt 102");
  } /*}}}*/

  std::deque<DiffContent> dst_diff_contents;
  { /*{{{*/
    DiffContent diff_content0;
    diff_content0.status = kValueNotEqual;
    diff_content0.key = "core_openai.name";
    diff_content0.type = kString;
    diff_content0.value.first_value_string = "openai001";
    diff_content0.value.second_value_string = "openai002";
    dst_diff_contents.push_back(diff_content0);
  } /*}}}*/
  { /*{{{*/
    DiffContent diff_content1;
    diff_content1.status = kValueNotEqual;
    diff_content1.key = "core_openai.ais1.1";
    diff_content1.type = kInt32;
    diff_content1.value.first_value_int32 = 0x7fffffe1;
    diff_content1.value.second_value_int32 = 0x7fffffff;
    dst_diff_contents.push_back(diff_content1);
  } /*}}}*/
  { /*{{{*/
    DiffContent diff_content2;
    diff_content2.status = kValueNotEqual;
    diff_content2.key = "core_openai.ais2.1";
    diff_content2.type = kUInt32;
    diff_content2.value.first_value_uint32 = 0xffffffe1;
    diff_content2.value.second_value_uint32 = 0xffffffff;
    dst_diff_contents.push_back(diff_content2);
  } /*}}}*/
  { /*{{{*/
    DiffContent diff_content3;
    diff_content3.status = kValueNotEqual;
    diff_content3.key = "core_openai.ais3.0";
    diff_content3.type = kInt64;
    diff_content3.value.first_value_int64 = 0x7fffffffffffffe1;
    diff_content3.value.second_value_int64 = 0x7fffffffffffffff;
    dst_diff_contents.push_back(diff_content3);
  } /*}}}*/
  { /*{{{*/
    DiffContent diff_content4;
    diff_content4.status = kValueNotEqual;
    diff_content4.key = "core_openai.ais4.0";
    diff_content4.type = kUInt64;
    diff_content4.value.first_value_uint64 = 0xffffffffffffffe1;
    diff_content4.value.second_value_uint64 = 0xffffffffffffffff;
    dst_diff_contents.push_back(diff_content4);
  } /*}}}*/
  { /*{{{*/
    DiffContent diff_content5;
    diff_content5.status = kValueNotEqual;
    diff_content5.key = "core_openai.ais5.1";
    diff_content5.type = kDouble;
    diff_content5.value.first_value_double = 44.123002;
    diff_content5.value.second_value_double = 44.123102;
    dst_diff_contents.push_back(diff_content5);
  } /*}}}*/
  { /*{{{*/
    DiffContent diff_content6;
    diff_content6.status = kValueNotEqual;
    diff_content6.key = "core_openai.ais6.0";
    diff_content6.type = kFloat;
    diff_content6.value.first_value_float = 55.19001;
    diff_content6.value.second_value_float = 55.19101;
    dst_diff_contents.push_back(diff_content6);
  } /*}}}*/
  { /*{{{*/
    DiffContent diff_content7;
    diff_content7.status = kValueNotEqual;
    diff_content7.key = "core_openai.ais7.1";
    diff_content7.type = kBool;
    diff_content7.value.first_value_bool = false;
    diff_content7.value.second_value_bool = true;
    dst_diff_contents.push_back(diff_content7);
  } /*}}}*/
  { /*{{{*/
    DiffContent diff_content8;
    diff_content8.status = kValueNotEqual;
    diff_content8.key = "core_openai.ais8.1";
    diff_content8.type = kEnum;
    diff_content8.value.first_value_enum = 103;
    diff_content8.value.second_value_enum = 101;
    dst_diff_contents.push_back(diff_content8);
  } /*}}}*/
  { /*{{{*/
    DiffContent diff_content9;
    diff_content9.status = kValueNotEqual;
    diff_content9.key = "core_openai.ais9.0";
    diff_content9.type = kString;
    diff_content9.value.first_value_string = "test for good ai 001";
    diff_content9.value.second_value_string = "test for good ai 101";
    dst_diff_contents.push_back(diff_content9);
  } /*}}}*/
  { /*{{{*/
    DiffContent diff_content0;
    diff_content0.status = kValueNotEqual;
    diff_content0.key = "core_openai.chat_gpts.1.name";
    diff_content0.type = kString;
    diff_content0.value.first_value_string = "gpt002";
    diff_content0.value.second_value_string = "gpt102";
    dst_diff_contents.push_back(diff_content0);
  } /*}}}*/
  { /*{{{*/
    DiffContent diff_content1;
    diff_content1.status = kValueNotEqual;
    diff_content1.key = "core_openai.chat_gpts.1.gpt1";
    diff_content1.type = kInt32;
    diff_content1.value.first_value_int32 = 0x7fffffe2;
    diff_content1.value.second_value_int32 = 0x7fffffff;
    dst_diff_contents.push_back(diff_content1);
  } /*}}}*/
  { /*{{{*/
    DiffContent diff_content2;
    diff_content2.status = kValueNotEqual;
    diff_content2.key = "core_openai.chat_gpts.1.gpt2";
    diff_content2.type = kUInt32;
    diff_content2.value.first_value_uint32 = 0xffffffe2;
    diff_content2.value.second_value_uint32 = 0xffffffff;
    dst_diff_contents.push_back(diff_content2);
  } /*}}}*/
  { /*{{{*/
    DiffContent diff_content3;
    diff_content3.status = kValueNotEqual;
    diff_content3.key = "core_openai.chat_gpts.1.gpt3";
    diff_content3.type = kInt64;
    diff_content3.value.first_value_int64 = 0x7fffffffffffffe2;
    diff_content3.value.second_value_int64 = 0x7fffffffffffffff;
    dst_diff_contents.push_back(diff_content3);
  } /*}}}*/
  { /*{{{*/
    DiffContent diff_content4;
    diff_content4.status = kValueNotEqual;
    diff_content4.key = "core_openai.chat_gpts.1.gpt4";
    diff_content4.type = kUInt64;
    diff_content4.value.first_value_uint64 = 0xffffffffffffffe2;
    diff_content4.value.second_value_uint64 = 0xffffffffffffffff;
    dst_diff_contents.push_back(diff_content4);
  } /*}}}*/
  { /*{{{*/
    DiffContent diff_content5;
    diff_content5.status = kValueNotEqual;
    diff_content5.key = "core_openai.chat_gpts.1.gpt5";
    diff_content5.type = kDouble;
    diff_content5.value.first_value_double = 11.123002;
    diff_content5.value.second_value_double = 11.123102;
    dst_diff_contents.push_back(diff_content5);
  } /*}}}*/
  { /*{{{*/
    DiffContent diff_content6;
    diff_content6.status = kValueNotEqual;
    diff_content6.key = "core_openai.chat_gpts.1.gpt6";
    diff_content6.type = kFloat;
    diff_content6.value.first_value_float = 11.0002;
    diff_content6.value.second_value_float = 11.0102;
    dst_diff_contents.push_back(diff_content6);
  } /*}}}*/
  { /*{{{*/
    DiffContent diff_content7;
    diff_content7.status = kValueNotEqual;
    diff_content7.key = "core_openai.chat_gpts.1.gpt7";
    diff_content7.type = kBool;
    diff_content7.value.first_value_bool = false;
    diff_content7.value.second_value_bool = true;
    dst_diff_contents.push_back(diff_content7);
  } /*}}}*/
  { /*{{{*/
    DiffContent diff_content8;
    diff_content8.status = kValueNotEqual;
    diff_content8.key = "core_openai.chat_gpts.1.gpt8";
    diff_content8.type = kEnum;
    diff_content8.value.first_value_enum = 101;
    diff_content8.value.second_value_enum = 102;
    dst_diff_contents.push_back(diff_content8);
  } /*}}}*/
  { /*{{{*/
    DiffContent diff_content9;
    diff_content9.status = kValueNotEqual;
    diff_content9.key = "core_openai.chat_gpts.1.gpt9";
    diff_content9.type = kString;
    diff_content9.value.first_value_string = "test for good gpt 002";
    diff_content9.value.second_value_string = "test for good gpt 102";
    dst_diff_contents.push_back(diff_content9);
  } /*}}}*/

  bool is_diff = false;
  std::deque<DiffContent> diff_contents;
  Code ret = proto::PBDiffWithOutExtension(person1, person2, &is_diff, &diff_contents);
  EXPECT_EQ(kOk, ret);
  EXPECT_EQ(true, is_diff);
  fprintf(stderr, "diff size:%zu\n", diff_contents.size());
  EXPECT_EQ(20, diff_contents.size());
  EXPECT_EQ(dst_diff_contents.size(), diff_contents.size());

  for (size_t i = 0; i < diff_contents.size(); ++i) {
    diff_contents[i].Print();
    fprintf(stderr, "\n");
    EXPECT_EQ(true, (diff_contents[i] == dst_diff_contents[i]));
  }

  // just check whether is different
  ret = proto::PBDiffWithOutExtension(person1, person2, &is_diff, NULL);
  EXPECT_EQ(kOk, ret);
  EXPECT_EQ(true, is_diff);
} /*}}}*/

TEST(PBDiffWithOutExtension, Test_Normal_Array_SecondEmptyArray) { /*{{{*/
  using namespace base;
  using namespace proto;

  model::Person person1;
  { /*{{{*/
    person1.set_name("zhangsan");
    person1.set_age(100);
    person1.set_country_name("China");
    person1.set_birthday(1);
    person1.set_height(1.7312389233245);
    person1.set_weight(105.413);
    person1.set_is_student(false);
    person1.set_resv1("good one");
    person1.set_resv2(0x1122334455667788);
    person1.set_resv3(0xfe11223344556677);
    person1.set_health_status(model::HS_GOOD);
    model::Addr *addr = person1.mutable_addrs();
    addr->set_place("New York");
    addr->set_num(1);
    model::CoreOpenAI *core_openai = person1.mutable_core_openai();
    core_openai->set_name("openai001");
    core_openai->add_ais1(0x77ffffe1);
    core_openai->add_ais1(0x7fffffe1);
    core_openai->add_ais2(0xf7ffffe1);
    core_openai->add_ais2(0xffffffe1);
    core_openai->add_ais3(0x7fffffffffffffe1);
    core_openai->add_ais3(0x7fffffffffffffe2);
    core_openai->add_ais4(0xffffffffffffffe1);
    core_openai->add_ais4(0xffffffffffffffe2);
    core_openai->add_ais5(44.123001);
    core_openai->add_ais5(44.123002);
    core_openai->add_ais6(55.19001);
    core_openai->add_ais6(55.19002);
    core_openai->add_ais7(true);
    core_openai->add_ais7(false);
    core_openai->add_ais8(model::HS_MID);
    core_openai->add_ais8(model::HS_BAD);
    core_openai->add_ais9("test for good ai 001");
    core_openai->add_ais9("test for good ai 002");
    model::ChatGPT *chat_gpt = core_openai->add_chat_gpts();
    chat_gpt->set_name("gpt001");
    chat_gpt->set_gpt1(0x7fffffe1);
    chat_gpt->set_gpt2(0xffffffe1);
    chat_gpt->set_gpt3(0x7fffffffffffffe1);
    chat_gpt->set_gpt4(0xffffffffffffffe1);
    chat_gpt->set_gpt5(11.123001);
    chat_gpt->set_gpt6(11.0001);
    chat_gpt->set_gpt7(true);
    chat_gpt->set_gpt8(model::HS_MID);
    chat_gpt->set_gpt9("test for good gpt 001");
    chat_gpt = core_openai->add_chat_gpts();
    chat_gpt->set_name("gpt002");
    chat_gpt->set_gpt1(0x7fffffe2);
    chat_gpt->set_gpt2(0xffffffe2);
    chat_gpt->set_gpt3(0x7fffffffffffffe2);
    chat_gpt->set_gpt4(0xffffffffffffffe2);
    chat_gpt->set_gpt5(11.123002);
    chat_gpt->set_gpt6(11.0002);
    chat_gpt->set_gpt7(false);
    chat_gpt->set_gpt8(model::HS_GOOD);
    chat_gpt->set_gpt9("test for good gpt 002");
  } /*}}}*/

  model::Person person2 = person1;
  { /*{{{*/
    person2.clear_core_openai();
    model::CoreOpenAI *core_openai = person2.mutable_core_openai();
    core_openai->set_name("openai002");
  } /*}}}*/

  std::deque<DiffContent> dst_diff_contents;
  { /*{{{*/
    DiffContent diff_content0;
    diff_content0.status = kValueNotEqual;
    diff_content0.key = "core_openai.name";
    diff_content0.type = kString;
    diff_content0.value.first_value_string = "openai001";
    diff_content0.value.second_value_string = "openai002";
    dst_diff_contents.push_back(diff_content0);
  } /*}}}*/
  { /*{{{*/
    DiffContent diff_content1;
    diff_content1.status = kArraySizeNotEqual;
    diff_content1.key = "core_openai.ais1";
    diff_content1.type = kInt32;
    diff_content1.first_array_size = 2;
    diff_content1.second_array_size = 0;
    dst_diff_contents.push_back(diff_content1);
  } /*}}}*/
  { /*{{{*/
    DiffContent diff_content2;
    diff_content2.status = kArraySizeNotEqual;
    diff_content2.key = "core_openai.ais2";
    diff_content2.type = kUInt32;
    diff_content2.first_array_size = 2;
    diff_content2.second_array_size = 0;
    dst_diff_contents.push_back(diff_content2);
  } /*}}}*/
  { /*{{{*/
    DiffContent diff_content3;
    diff_content3.status = kArraySizeNotEqual;
    diff_content3.key = "core_openai.ais3";
    diff_content3.type = kInt64;
    diff_content3.first_array_size = 2;
    diff_content3.second_array_size = 0;
    dst_diff_contents.push_back(diff_content3);
  } /*}}}*/
  { /*{{{*/
    DiffContent diff_content4;
    diff_content4.status = kArraySizeNotEqual;
    diff_content4.key = "core_openai.ais4";
    diff_content4.type = kUInt64;
    diff_content4.first_array_size = 2;
    diff_content4.second_array_size = 0;
    dst_diff_contents.push_back(diff_content4);
  } /*}}}*/
  { /*{{{*/
    DiffContent diff_content5;
    diff_content5.status = kArraySizeNotEqual;
    diff_content5.key = "core_openai.ais5";
    diff_content5.type = kDouble;
    diff_content5.first_array_size = 2;
    diff_content5.second_array_size = 0;
    dst_diff_contents.push_back(diff_content5);
  } /*}}}*/
  { /*{{{*/
    DiffContent diff_content6;
    diff_content6.status = kArraySizeNotEqual;
    diff_content6.key = "core_openai.ais6";
    diff_content6.type = kFloat;
    diff_content6.first_array_size = 2;
    diff_content6.second_array_size = 0;
    dst_diff_contents.push_back(diff_content6);
  } /*}}}*/
  { /*{{{*/
    DiffContent diff_content7;
    diff_content7.status = kArraySizeNotEqual;
    diff_content7.key = "core_openai.ais7";
    diff_content7.type = kBool;
    diff_content7.first_array_size = 2;
    diff_content7.second_array_size = 0;
    dst_diff_contents.push_back(diff_content7);
  } /*}}}*/
  { /*{{{*/
    DiffContent diff_content8;
    diff_content8.status = kArraySizeNotEqual;
    diff_content8.key = "core_openai.ais8";
    diff_content8.type = kEnum;
    diff_content8.first_array_size = 2;
    diff_content8.second_array_size = 0;
    dst_diff_contents.push_back(diff_content8);
  } /*}}}*/
  { /*{{{*/
    DiffContent diff_content9;
    diff_content9.status = kArraySizeNotEqual;
    diff_content9.key = "core_openai.ais9";
    diff_content9.type = kString;
    diff_content9.first_array_size = 2;
    diff_content9.second_array_size = 0;
    dst_diff_contents.push_back(diff_content9);
  } /*}}}*/
  { /*{{{*/
    DiffContent diff_content10;
    diff_content10.status = kArraySizeNotEqual;
    diff_content10.key = "core_openai.chat_gpts";
    diff_content10.type = kObject;
    diff_content10.first_array_size = 2;
    diff_content10.second_array_size = 0;
    dst_diff_contents.push_back(diff_content10);
  } /*}}}*/

  bool is_diff = false;
  std::deque<DiffContent> diff_contents;
  Code ret = proto::PBDiffWithOutExtension(person1, person2, &is_diff, &diff_contents);
  EXPECT_EQ(kOk, ret);
  EXPECT_EQ(true, is_diff);
  fprintf(stderr, "diff size:%zu\n", diff_contents.size());
  EXPECT_EQ(11, diff_contents.size());
  EXPECT_EQ(dst_diff_contents.size(), diff_contents.size());

  for (size_t i = 0; i < diff_contents.size(); ++i) {
    diff_contents[i].Print();
    fprintf(stderr, "\n");
    EXPECT_EQ(true, (diff_contents[i] == dst_diff_contents[i]));
  }

  // just check whether is different
  ret = proto::PBDiffWithOutExtension(person1, person2, &is_diff, NULL);
  EXPECT_EQ(kOk, ret);
  EXPECT_EQ(true, is_diff);
} /*}}}*/

TEST(PBDiffWithOutExtension, Test_Normal_Array_FirstEmtpyArray) { /*{{{*/
  using namespace base;
  using namespace proto;

  model::Person person1;
  { /*{{{*/
    person1.set_name("zhangsan");
    person1.set_age(100);
    person1.set_country_name("China");
    person1.set_birthday(1);
    person1.set_height(1.7312389233245);
    person1.set_weight(105.413);
    person1.set_is_student(false);
    person1.set_resv1("good one");
    person1.set_resv2(0x1122334455667788);
    person1.set_resv3(0xfe11223344556677);
    person1.set_health_status(model::HS_GOOD);
    model::Addr *addr = person1.mutable_addrs();
    addr->set_place("New York");
    addr->set_num(1);
    model::CoreOpenAI *core_openai = person1.mutable_core_openai();
    core_openai->set_name("openai001");
  } /*}}}*/

  model::Person person2 = person1;
  { /*{{{*/
    person2.clear_core_openai();
    model::CoreOpenAI *core_openai = person2.mutable_core_openai();
    core_openai->set_name("openai002");
    core_openai->add_ais1(0x77ffffe1);
    core_openai->add_ais1(0x7fffffff);
    core_openai->add_ais2(0xf7ffffe1);
    core_openai->add_ais2(0xffffffff);
    core_openai->add_ais3(0x7fffffffffffffff);
    core_openai->add_ais3(0x7fffffffffffffe2);
    core_openai->add_ais4(0xffffffffffffffff);
    core_openai->add_ais4(0xffffffffffffffe2);
    core_openai->add_ais5(44.123001);
    core_openai->add_ais5(44.123102);
    core_openai->add_ais6(55.19101);
    core_openai->add_ais6(55.19002);
    core_openai->add_ais7(true);
    core_openai->add_ais7(true);
    core_openai->add_ais8(model::HS_MID);
    core_openai->add_ais8(model::HS_GOOD);
    core_openai->add_ais9("test for good ai 101");
    core_openai->add_ais9("test for good ai 002");
    model::ChatGPT *chat_gpt = core_openai->add_chat_gpts();
    chat_gpt->set_name("gpt001");
    chat_gpt->set_gpt1(0x7fffffe1);
    chat_gpt->set_gpt2(0xffffffe1);
    chat_gpt->set_gpt3(0x7fffffffffffffe1);
    chat_gpt->set_gpt4(0xffffffffffffffe1);
    chat_gpt->set_gpt5(11.123001);
    chat_gpt->set_gpt6(11.0001);
    chat_gpt->set_gpt7(true);
    chat_gpt->set_gpt8(model::HS_MID);
    chat_gpt->set_gpt9("test for good gpt 001");
    chat_gpt = core_openai->add_chat_gpts();
    chat_gpt->set_name("gpt102");
    chat_gpt->set_gpt1(0x7fffffff);
    chat_gpt->set_gpt2(0xffffffff);
    chat_gpt->set_gpt3(0x7fffffffffffffff);
    chat_gpt->set_gpt4(0xffffffffffffffff);
    chat_gpt->set_gpt5(11.123102);
    chat_gpt->set_gpt6(11.0102);
    chat_gpt->set_gpt7(true);
    chat_gpt->set_gpt8(model::HS_MID);
    chat_gpt->set_gpt9("test for good gpt 102");
  } /*}}}*/

  std::deque<DiffContent> dst_diff_contents;
  { /*{{{*/
    DiffContent diff_content0;
    diff_content0.status = kValueNotEqual;
    diff_content0.key = "core_openai.name";
    diff_content0.type = kString;
    diff_content0.value.first_value_string = "openai001";
    diff_content0.value.second_value_string = "openai002";
    dst_diff_contents.push_back(diff_content0);
  } /*}}}*/
  { /*{{{*/
    DiffContent diff_content1;
    diff_content1.status = kArraySizeNotEqual;
    diff_content1.key = "core_openai.ais1";
    diff_content1.type = kInt32;
    diff_content1.first_array_size = 0;
    diff_content1.second_array_size = 2;
    dst_diff_contents.push_back(diff_content1);
  } /*}}}*/
  { /*{{{*/
    DiffContent diff_content2;
    diff_content2.status = kArraySizeNotEqual;
    diff_content2.key = "core_openai.ais2";
    diff_content2.type = kUInt32;
    diff_content2.first_array_size = 0;
    diff_content2.second_array_size = 2;
    dst_diff_contents.push_back(diff_content2);
  } /*}}}*/
  { /*{{{*/
    DiffContent diff_content3;
    diff_content3.status = kArraySizeNotEqual;
    diff_content3.key = "core_openai.ais3";
    diff_content3.type = kInt64;
    diff_content3.first_array_size = 0;
    diff_content3.second_array_size = 2;
    dst_diff_contents.push_back(diff_content3);
  } /*}}}*/
  { /*{{{*/
    DiffContent diff_content4;
    diff_content4.status = kArraySizeNotEqual;
    diff_content4.key = "core_openai.ais4";
    diff_content4.type = kUInt64;
    diff_content4.first_array_size = 0;
    diff_content4.second_array_size = 2;
    dst_diff_contents.push_back(diff_content4);
  } /*}}}*/
  { /*{{{*/
    DiffContent diff_content5;
    diff_content5.status = kArraySizeNotEqual;
    diff_content5.key = "core_openai.ais5";
    diff_content5.type = kDouble;
    diff_content5.first_array_size = 0;
    diff_content5.second_array_size = 2;
    dst_diff_contents.push_back(diff_content5);
  } /*}}}*/
  { /*{{{*/
    DiffContent diff_content6;
    diff_content6.status = kArraySizeNotEqual;
    diff_content6.key = "core_openai.ais6";
    diff_content6.type = kFloat;
    diff_content6.first_array_size = 0;
    diff_content6.second_array_size = 2;
    dst_diff_contents.push_back(diff_content6);
  } /*}}}*/
  { /*{{{*/
    DiffContent diff_content7;
    diff_content7.status = kArraySizeNotEqual;
    diff_content7.key = "core_openai.ais7";
    diff_content7.type = kBool;
    diff_content7.first_array_size = 0;
    diff_content7.second_array_size = 2;
    dst_diff_contents.push_back(diff_content7);
  } /*}}}*/
  { /*{{{*/
    DiffContent diff_content8;
    diff_content8.status = kArraySizeNotEqual;
    diff_content8.key = "core_openai.ais8";
    diff_content8.type = kEnum;
    diff_content8.first_array_size = 0;
    diff_content8.second_array_size = 2;
    dst_diff_contents.push_back(diff_content8);
  } /*}}}*/
  { /*{{{*/
    DiffContent diff_content9;
    diff_content9.status = kArraySizeNotEqual;
    diff_content9.key = "core_openai.ais9";
    diff_content9.type = kString;
    diff_content9.first_array_size = 0;
    diff_content9.second_array_size = 2;
    dst_diff_contents.push_back(diff_content9);
  } /*}}}*/
  { /*{{{*/
    DiffContent diff_content10;
    diff_content10.status = kArraySizeNotEqual;
    diff_content10.key = "core_openai.chat_gpts";
    diff_content10.type = kObject;
    diff_content10.first_array_size = 0;
    diff_content10.second_array_size = 2;
    dst_diff_contents.push_back(diff_content10);
  } /*}}}*/

  bool is_diff = false;
  std::deque<DiffContent> diff_contents;
  Code ret = proto::PBDiffWithOutExtension(person1, person2, &is_diff, &diff_contents);
  EXPECT_EQ(kOk, ret);
  EXPECT_EQ(true, is_diff);
  fprintf(stderr, "diff size:%zu\n", diff_contents.size());
  EXPECT_EQ(11, diff_contents.size());
  EXPECT_EQ(dst_diff_contents.size(), diff_contents.size());

  for (size_t i = 0; i < diff_contents.size(); ++i) {
    diff_contents[i].Print();
    fprintf(stderr, "\n");
    EXPECT_EQ(true, (diff_contents[i] == dst_diff_contents[i]));
  }

  // just check whether is different
  ret = proto::PBDiffWithOutExtension(person1, person2, &is_diff, NULL);
  EXPECT_EQ(kOk, ret);
  EXPECT_EQ(true, is_diff);
} /*}}}*/

TEST(PBDiffWithOutExtension, Test_Normal_Array_ArraySizeNotEqual) { /*{{{*/
  using namespace base;
  using namespace proto;

  model::Person person1;
  { /*{{{*/
    person1.set_name("zhangsan");
    person1.set_age(100);
    person1.set_country_name("China");
    person1.set_birthday(1);
    person1.set_height(1.7312389233245);
    person1.set_weight(105.413);
    person1.set_is_student(false);
    person1.set_resv1("good one");
    person1.set_resv2(0x1122334455667788);
    person1.set_resv3(0xfe11223344556677);
    person1.set_health_status(model::HS_GOOD);
    model::Addr *addr = person1.mutable_addrs();
    addr->set_place("New York");
    addr->set_num(1);
    model::CoreOpenAI *core_openai = person1.mutable_core_openai();
    core_openai->set_name("openai001");
    core_openai->add_ais1(0x77ffffe1);
    core_openai->add_ais1(0x7fffffe1);
    core_openai->add_ais2(0xf7ffffe1);
    core_openai->add_ais2(0xffffffe1);
    core_openai->add_ais3(0x7fffffffffffffe1);
    core_openai->add_ais3(0x7fffffffffffffe2);
    core_openai->add_ais4(0xffffffffffffffe1);
    core_openai->add_ais4(0xffffffffffffffe2);
    core_openai->add_ais5(44.123001);
    core_openai->add_ais5(44.123002);
    core_openai->add_ais6(55.19001);
    core_openai->add_ais6(55.19002);
    core_openai->add_ais7(true);
    core_openai->add_ais7(false);
    core_openai->add_ais8(model::HS_MID);
    core_openai->add_ais8(model::HS_BAD);
    core_openai->add_ais9("test for good ai 001");
    core_openai->add_ais9("test for good ai 002");
    model::ChatGPT *chat_gpt = core_openai->add_chat_gpts();
    chat_gpt->set_name("gpt001");
    chat_gpt->set_gpt1(0x7fffffe1);
    chat_gpt->set_gpt2(0xffffffe1);
    chat_gpt->set_gpt3(0x7fffffffffffffe1);
    chat_gpt->set_gpt4(0xffffffffffffffe1);
    chat_gpt->set_gpt5(11.123001);
    chat_gpt->set_gpt6(11.0001);
    chat_gpt->set_gpt7(true);
    chat_gpt->set_gpt8(model::HS_MID);
    chat_gpt->set_gpt9("test for good gpt 001");
    chat_gpt = core_openai->add_chat_gpts();
    chat_gpt->set_name("gpt002");
    chat_gpt->set_gpt1(0x7fffffe2);
    chat_gpt->set_gpt2(0xffffffe2);
    chat_gpt->set_gpt3(0x7fffffffffffffe2);
    chat_gpt->set_gpt4(0xffffffffffffffe2);
    chat_gpt->set_gpt5(11.123002);
    chat_gpt->set_gpt6(11.0002);
    chat_gpt->set_gpt7(false);
    chat_gpt->set_gpt8(model::HS_GOOD);
    chat_gpt->set_gpt9("test for good gpt 002");
  } /*}}}*/

  model::Person person2 = person1;
  { /*{{{*/
    person2.clear_core_openai();
    model::CoreOpenAI *core_openai = person2.mutable_core_openai();
    core_openai->set_name("openai002");
    core_openai->add_ais1(0x77ffffe1);
    core_openai->add_ais2(0xf7ffffe1);
    core_openai->add_ais3(0x7fffffffffffffff);
    core_openai->add_ais4(0xffffffffffffffff);
    core_openai->add_ais5(44.123001);
    core_openai->add_ais6(55.19101);
    core_openai->add_ais7(true);
    core_openai->add_ais8(model::HS_MID);
    core_openai->add_ais9("test for good ai 101");
    model::ChatGPT *chat_gpt = core_openai->add_chat_gpts();
    chat_gpt->set_name("gpt001");
    chat_gpt->set_gpt1(0x7fffffe1);
    chat_gpt->set_gpt2(0xffffffe1);
    chat_gpt->set_gpt3(0x7fffffffffffffe1);
    chat_gpt->set_gpt4(0xffffffffffffffe1);
    chat_gpt->set_gpt5(11.123001);
    chat_gpt->set_gpt6(11.0001);
    chat_gpt->set_gpt7(true);
    chat_gpt->set_gpt8(model::HS_MID);
    chat_gpt->set_gpt9("test for good gpt 001");
  } /*}}}*/

  std::deque<DiffContent> dst_diff_contents;
  { /*{{{*/
    DiffContent diff_content0;
    diff_content0.status = kValueNotEqual;
    diff_content0.key = "core_openai.name";
    diff_content0.type = kString;
    diff_content0.value.first_value_string = "openai001";
    diff_content0.value.second_value_string = "openai002";
    dst_diff_contents.push_back(diff_content0);
  } /*}}}*/
  { /*{{{*/
    DiffContent diff_content1;
    diff_content1.status = kArraySizeNotEqual;
    diff_content1.key = "core_openai.ais1";
    diff_content1.type = kInt32;
    diff_content1.first_array_size = 2;
    diff_content1.second_array_size = 1;
    dst_diff_contents.push_back(diff_content1);
  } /*}}}*/
  { /*{{{*/
    DiffContent diff_content2;
    diff_content2.status = kArraySizeNotEqual;
    diff_content2.key = "core_openai.ais2";
    diff_content2.type = kUInt32;
    diff_content2.first_array_size = 2;
    diff_content2.second_array_size = 1;
    dst_diff_contents.push_back(diff_content2);
  } /*}}}*/
  { /*{{{*/
    DiffContent diff_content3;
    diff_content3.status = kArraySizeNotEqual;
    diff_content3.key = "core_openai.ais3";
    diff_content3.type = kInt64;
    diff_content3.first_array_size = 2;
    diff_content3.second_array_size = 1;
    dst_diff_contents.push_back(diff_content3);
  } /*}}}*/
  { /*{{{*/
    DiffContent diff_content4;
    diff_content4.status = kArraySizeNotEqual;
    diff_content4.key = "core_openai.ais4";
    diff_content4.type = kUInt64;
    diff_content4.first_array_size = 2;
    diff_content4.second_array_size = 1;
    dst_diff_contents.push_back(diff_content4);
  } /*}}}*/
  { /*{{{*/
    DiffContent diff_content5;
    diff_content5.status = kArraySizeNotEqual;
    diff_content5.key = "core_openai.ais5";
    diff_content5.type = kDouble;
    diff_content5.first_array_size = 2;
    diff_content5.second_array_size = 1;
    dst_diff_contents.push_back(diff_content5);
  } /*}}}*/
  { /*{{{*/
    DiffContent diff_content6;
    diff_content6.status = kArraySizeNotEqual;
    diff_content6.key = "core_openai.ais6";
    diff_content6.type = kFloat;
    diff_content6.first_array_size = 2;
    diff_content6.second_array_size = 1;
    dst_diff_contents.push_back(diff_content6);
  } /*}}}*/
  { /*{{{*/
    DiffContent diff_content7;
    diff_content7.status = kArraySizeNotEqual;
    diff_content7.key = "core_openai.ais7";
    diff_content7.type = kBool;
    diff_content7.first_array_size = 2;
    diff_content7.second_array_size = 1;
    dst_diff_contents.push_back(diff_content7);
  } /*}}}*/
  { /*{{{*/
    DiffContent diff_content8;
    diff_content8.status = kArraySizeNotEqual;
    diff_content8.key = "core_openai.ais8";
    diff_content8.type = kEnum;
    diff_content8.first_array_size = 2;
    diff_content8.second_array_size = 1;
    dst_diff_contents.push_back(diff_content8);
  } /*}}}*/
  { /*{{{*/
    DiffContent diff_content9;
    diff_content9.status = kArraySizeNotEqual;
    diff_content9.key = "core_openai.ais9";
    diff_content9.type = kString;
    diff_content9.first_array_size = 2;
    diff_content9.second_array_size = 1;
    dst_diff_contents.push_back(diff_content9);
  } /*}}}*/
  { /*{{{*/
    DiffContent diff_content10;
    diff_content10.status = kArraySizeNotEqual;
    diff_content10.key = "core_openai.chat_gpts";
    diff_content10.type = kObject;
    diff_content10.first_array_size = 2;
    diff_content10.second_array_size = 1;
    dst_diff_contents.push_back(diff_content10);
  } /*}}}*/

  bool is_diff = false;
  std::deque<DiffContent> diff_contents;
  Code ret = proto::PBDiffWithOutExtension(person1, person2, &is_diff, &diff_contents);
  EXPECT_EQ(kOk, ret);
  EXPECT_EQ(true, is_diff);
  fprintf(stderr, "diff size:%zu\n", diff_contents.size());
  EXPECT_EQ(11, diff_contents.size());
  EXPECT_EQ(dst_diff_contents.size(), diff_contents.size());

  for (size_t i = 0; i < diff_contents.size(); ++i) {
    diff_contents[i].Print();
    fprintf(stderr, "\n");
    EXPECT_EQ(true, (diff_contents[i] == dst_diff_contents[i]));
  }

  // just check whether is different
  ret = proto::PBDiffWithOutExtension(person1, person2, &is_diff, NULL);
  EXPECT_EQ(kOk, ret);
  EXPECT_EQ(true, is_diff);
} /*}}}*/

TEST(PBDiffWithOutExtension, Test_Normal_SameType_JustCheckIsDiff) { /*{{{*/
  using namespace base;
  using namespace proto;

  model::Person person1;
  { /*{{{*/
    person1.set_name("zhangsan");
    person1.set_age(100);
    person1.set_country_name("China");
    person1.set_birthday(1);
    person1.set_height(1.7312389233245);
    person1.set_weight(105.413);
    person1.set_is_student(false);
    person1.set_resv1("good one");
    person1.set_resv2(0x1122334455667788);
    person1.set_resv3(0xfe11223344556677);
    person1.set_health_status(model::HS_GOOD);
    model::Addr *addr = person1.mutable_addrs();
    addr->set_place("New York");
    addr->set_num(1);
    model::OpenAI *openai = person1.mutable_openai();
    openai->set_name("lisi hello one");
    openai->set_ai1(0x7fffffff);
    openai->set_ai2(0xffffffff);
    openai->set_ai3(0x7fffffffffffffff);
    openai->set_ai4(0xffffffffffffffff);
    openai->set_ai5(1231.123123123123);
    openai->set_ai6(89234.8912123);
    openai->set_ai7(true);
    openai->set_ai8(model::HS_MID);
    openai->set_ai9("test for good and you may go to this, could you give me some advise");
    model::ChatGPT *chat_gpt = openai->mutable_chat_gpt();
    chat_gpt->set_name("lisi hello two");
    chat_gpt->set_gpt1(0x7fffffff);
    chat_gpt->set_gpt2(0xffffffff);
    chat_gpt->set_gpt3(0x7fffffffffffffff);
    chat_gpt->set_gpt4(0xffffffffffffffff);
    chat_gpt->set_gpt5(1231.123123123123);
    chat_gpt->set_gpt6(89234.8912123);
    chat_gpt->set_gpt7(true);
    chat_gpt->set_gpt8(model::HS_MID);
    chat_gpt->set_gpt9("test for good and you may go to this, could you give me some advise");
  } /*}}}*/

  model::Person person2 = person1;

  bool is_diff = false;
  Code ret = proto::PBDiffWithOutExtension(person1, person2, &is_diff, NULL);
  EXPECT_EQ(kOk, ret);
  EXPECT_EQ(false, is_diff);
} /*}}}*/

TEST(PBDiffWithOutExtension, Test_Normal_DiffType_JustCheckIsDiff) { /*{{{*/
  using namespace base;
  using namespace proto;

  model::Person person1;
  { /*{{{*/
    person1.set_name("zhangsan");
    person1.set_age(100);
    person1.set_country_name("China");
    person1.set_birthday(1);
    person1.set_height(1.7312389233245);
    person1.set_weight(105.413);
    person1.set_is_student(false);
    person1.set_resv1("good one");
    person1.set_resv2(0x1122334455667788);
    person1.set_resv3(0xfe11223344556677);
    person1.set_health_status(model::HS_GOOD);
    model::Addr *addr = person1.mutable_addrs();
    addr->set_place("New York");
    addr->set_num(1);
    model::OpenAI *openai = person1.mutable_openai();
    openai->set_name("lisi hello one");
    openai->set_ai1(0x7fffffff);
    openai->set_ai2(0xffffffff);
    openai->set_ai3(0x7fffffffffffffff);
    openai->set_ai4(0xffffffffffffffff);
    openai->set_ai5(1231.123123123123);
    openai->set_ai6(89234.8912123);
    openai->set_ai7(true);
    openai->set_ai8(model::HS_MID);
    openai->set_ai9("test for good and you may go to this, could you give me some advise");
    model::ChatGPT *chat_gpt = openai->mutable_chat_gpt();
    chat_gpt->set_name("gpt1");
    chat_gpt->set_gpt1(0x7fffffff);
    chat_gpt->set_gpt2(0xffffffff);
    chat_gpt->set_gpt3(0x7fffffffffffffff);
    chat_gpt->set_gpt4(0xffffffffffffffff);
    chat_gpt->set_gpt5(12.123113);
    chat_gpt->set_gpt6(89.8923);
    chat_gpt->set_gpt7(true);
    chat_gpt->set_gpt8(model::HS_MID);
    chat_gpt->set_gpt9("test for gpt1");
  } /*}}}*/

  model::Person person2 = person1;
  { /*{{{*/
    model::ChatGPT *chat_gpt = person2.mutable_openai()->mutable_chat_gpt();
    chat_gpt->set_name("gpt2");
    chat_gpt->set_gpt1(0x7ffffffe);
    chat_gpt->set_gpt2(0xfffffffe);
    chat_gpt->set_gpt3(0x7ffffffffffffffe);
    chat_gpt->set_gpt4(0xfffffffffffffffe);
    chat_gpt->set_gpt5(12.123114);
    chat_gpt->set_gpt6(89.8924);
    chat_gpt->set_gpt7(false);
    chat_gpt->set_gpt8(model::HS_BAD);
    chat_gpt->set_gpt9("test for gpt2");
  } /*}}}*/

  std::deque<DiffContent> dst_diff_contents;
  { /*{{{*/
    DiffContent diff_content0;
    diff_content0.status = kValueNotEqual;
    diff_content0.key = "openai.chat_gpt.name";
    diff_content0.type = kString;
    diff_content0.value.first_value_string = "gpt1";
    diff_content0.value.second_value_string = "gpt2";
    dst_diff_contents.push_back(diff_content0);
  } /*}}}*/
  { /*{{{*/
    DiffContent diff_content1;
    diff_content1.status = kValueNotEqual;
    diff_content1.key = "openai.chat_gpt.gpt1";
    diff_content1.type = kInt32;
    diff_content1.value.first_value_int32 = 0x7fffffff;
    diff_content1.value.second_value_int32 = 0x7ffffffe;
    dst_diff_contents.push_back(diff_content1);
  } /*}}}*/
  { /*{{{*/
    DiffContent diff_content2;
    diff_content2.status = kValueNotEqual;
    diff_content2.key = "openai.chat_gpt.gpt2";
    diff_content2.type = kUInt32;
    diff_content2.value.first_value_uint32 = 0xffffffff;
    diff_content2.value.second_value_uint32 = 0xfffffffe;
    dst_diff_contents.push_back(diff_content2);
  } /*}}}*/
  { /*{{{*/
    DiffContent diff_content3;
    diff_content3.status = kValueNotEqual;
    diff_content3.key = "openai.chat_gpt.gpt3";
    diff_content3.type = kInt64;
    diff_content3.value.first_value_int64 = 0x7fffffffffffffff;
    diff_content3.value.second_value_int64 = 0x7ffffffffffffffe;
    dst_diff_contents.push_back(diff_content3);
  } /*}}}*/
  { /*{{{*/
    DiffContent diff_content4;
    diff_content4.status = kValueNotEqual;
    diff_content4.key = "openai.chat_gpt.gpt4";
    diff_content4.type = kUInt64;
    diff_content4.value.first_value_uint64 = 0xffffffffffffffff;
    diff_content4.value.second_value_uint64 = 0xfffffffffffffffe;
    dst_diff_contents.push_back(diff_content4);
  } /*}}}*/
  { /*{{{*/
    DiffContent diff_content5;
    diff_content5.status = kValueNotEqual;
    diff_content5.key = "openai.chat_gpt.gpt5";
    diff_content5.type = kDouble;
    diff_content5.value.first_value_double = 12.123113;
    diff_content5.value.second_value_double = 12.123114;
    dst_diff_contents.push_back(diff_content5);
  } /*}}}*/
  { /*{{{*/
    DiffContent diff_content6;
    diff_content6.status = kValueNotEqual;
    diff_content6.key = "openai.chat_gpt.gpt6";
    diff_content6.type = kFloat;
    diff_content6.value.first_value_float = 89.8923;
    diff_content6.value.second_value_float = 89.8924;
    dst_diff_contents.push_back(diff_content6);
  } /*}}}*/
  { /*{{{*/
    DiffContent diff_content7;
    diff_content7.status = kValueNotEqual;
    diff_content7.key = "openai.chat_gpt.gpt7";
    diff_content7.type = kBool;
    diff_content7.value.first_value_bool = true;
    diff_content7.value.second_value_bool = false;
    dst_diff_contents.push_back(diff_content7);
  } /*}}}*/
  { /*{{{*/
    DiffContent diff_content8;
    diff_content8.status = kValueNotEqual;
    diff_content8.key = "openai.chat_gpt.gpt8";
    diff_content8.type = kEnum;
    diff_content8.value.first_value_enum = 102;
    diff_content8.value.second_value_enum = 103;
    dst_diff_contents.push_back(diff_content8);
  } /*}}}*/
  { /*{{{*/
    DiffContent diff_content9;
    diff_content9.status = kValueNotEqual;
    diff_content9.key = "openai.chat_gpt.gpt9";
    diff_content9.type = kString;
    diff_content9.value.first_value_string = "test for gpt1";
    diff_content9.value.second_value_string = "test for gpt2";
    dst_diff_contents.push_back(diff_content9);
  } /*}}}*/

  bool is_diff = false;
  Code ret = proto::PBDiffWithOutExtension(person1, person2, &is_diff, NULL);
  EXPECT_EQ(kOk, ret);
  EXPECT_EQ(true, is_diff);
} /*}}}*/

TEST(PBDiffWithOutExtension, Test_Exception_DiffType) { /*{{{*/
  using namespace base;
  using namespace proto;

  model::Person person;
  person.set_name("zhangsan");

  model::MoreParams more_params;
  more_params.set_age(1);

  bool is_diff = false;
  std::deque<DiffContent> diff_contents;
  Code ret = proto::PBDiffWithOutExtension(person, more_params, &is_diff, &diff_contents);
  EXPECT_EQ(kOk, ret);

  EXPECT_EQ(true, is_diff);
  EXPECT_EQ(1, diff_contents.size());
  EXPECT_EQ(kPBTypeNotEqual, diff_contents[0].status);
  EXPECT_EQ("model.Person", diff_contents[0].first_type);
  EXPECT_EQ("model.MoreParams", diff_contents[0].second_type);
  fprintf(stderr, "first type:%s, second type:%s\n", diff_contents[0].first_type.c_str(),
          diff_contents[0].second_type.c_str());

  // fprintf(stderr, "pb_str:\n%s\n", person.DebugString().c_str());
  // fprintf(stderr, "size:%zu, json:       %s\n", json.size(), json.c_str());
  // fprintf(stderr, "size:%zu, expect_json:%s\n", expect_json.size(), expect_json.c_str());
} /*}}}*/

TEST_D(InitJsonValue, Test_Normal_DealFromFile, "验证将json中指定key重新初始化") { /*{{{*/
  using namespace base;
  using namespace proto;

  std::string src_file = "../data/json/test_init_json.txt";
  FILE *src_fp = fopen(src_file.c_str(), "r");
  EXPECT_NEQ(NULL, src_fp);
  std::string cnt;
  Code ret = PumpWholeData(&cnt, src_fp);
  EXPECT_EQ(kOk, ret);

  rapidjson::Document d;
  d.Parse(cnt.c_str(), cnt.size());
  EXPECT_EQ(false, d.HasParseError());
  EXPECT_EQ(true, d.IsObject());
  EXPECT_EQ(true, d.HasMember("test_cases"));
  EXPECT_EQ(true, d["test_cases"].IsArray());

  rapidjson::Value::ConstValueIterator it = d["test_cases"].Begin();
  for (; it != d["test_cases"].End(); ++it) {
    EXPECT_EQ(true, it->IsObject());
    // NOTE:htt, 初始化字符串内容
    EXPECT_EQ(true, it->HasMember("src"));
    EXPECT_EQ(true, (*it)["src"].IsObject());

    // NOTE:htt, 需要初始化的keys列表
    EXPECT_EQ(true, it->HasMember("init_keys"));
    EXPECT_EQ(true, (*it)["init_keys"].IsArray());

    // NOTE:htt, 初始化函数处理返回值
    EXPECT_EQ(true, it->HasMember("ret"));
    EXPECT_EQ(true, (*it)["ret"].IsInt());

    // NOTE:htt, 初始化成功后对应处理的字符串
    EXPECT_EQ(true, it->HasMember("expect"));
    EXPECT_EQ(true, (*it)["expect"].IsObject());

    std::set<std::string> init_keys;
    rapidjson::Value::ConstValueIterator v_it = (*it)["init_keys"].Begin();
    for (; v_it != (*it)["init_keys"].End(); ++v_it) {
      EXPECT_EQ(true, v_it->IsString());

      init_keys.insert(std::string(v_it->GetString(), v_it->GetStringLength()));
    }

    std::string src_str = GetStringFromJson((*it)["src"]);
    std::string expect_str = GetStringFromJson((*it)["expect"]);

    std::string dst_str;
    ret = InitJsonValue(src_str, init_keys, &dst_str);
    int json_ret = (*it)["ret"].GetInt();
    EXPECT_EQ(json_ret, ret);
    if (json_ret == kOk) {
      EXPECT_EQ(expect_str, dst_str);
    }

    fprintf(stderr, "src_str:%s\nexpect_str:%s\ndst_str:%s\n\n", src_str.c_str(), expect_str.c_str(), dst_str.c_str());
  }
} /*}}}*/

TEST_D(InitJsonValue, Test_Normal_DealFromFile_Test, "验证将json中特殊字符") { /*{{{*/
  using namespace base;
  using namespace proto;

  std::string check_str;

  // NOTE:htt, 正常字符
  check_str = "chatGPT张三";
  bool is_valid = false;
  Code ret = IsJsonValueValidEncoding(check_str, &is_valid);
  EXPECT_EQ(base::kOk, ret);
  EXPECT_EQ(true, is_valid);

  // NOTE:htt, 正常字符
  check_str = "";
  check_str.append(1, 0xe6);
  check_str.append(1, 0x98);
  check_str.append(1, 0x9f);
  ret = IsJsonValueValidEncoding(check_str, &is_valid);
  EXPECT_EQ(base::kOk, ret);
  EXPECT_EQ(true, is_valid);

  // NOTE:htt, 非UTF8字符
  check_str = "";
  check_str.append(1, 0xe6);
  check_str.append(1, 0x98);
  ret = IsJsonValueValidEncoding(check_str, &is_valid);
  EXPECT_EQ(base::kOk, ret);
  EXPECT_EQ(false, is_valid);

  // NOTE:htt, 控制字符
  check_str = "";
  check_str.append(1, 0x1a);
  ret = IsJsonValueValidEncoding(check_str, &is_valid);
  EXPECT_EQ(base::kOk, ret);
  EXPECT_EQ(false, is_valid);
} /*}}}*/

TEST_D(InitJsonValue, Test_Normal_CheckKeyReplace_Test, "验证json中key替换") { /*{{{*/
  using namespace base;
  using namespace proto;

  std::string source_json =
      "{\"name\":\"zhangsan\",\"age\":100,\"country_name\":\"China\",\"birthday\":1,\"height\":1."
      "73,\"weight\":105.41300201416016,\"is_student\":false,\"resv1\":\"good "
      "one\",\"resv2\":1234605616436508552,\"resv3\":-139292509886650761}";
  std::string expect_json =
      "{\"name\":\"zhangsan\",\"age\":100,\"country_name\":\"American\",\"birthday\":1,\"height\":"
      "1."
      "73,\"weight\":105.41300201416016,\"is_student\":false,\"resv1\":\"good "
      "one\",\"resv2\":1234605616436508552,\"resv3\":-139292509886650761}";

  rapidjson::Document d;
  d.Parse(source_json.c_str(), source_json.size());
  EXPECT_EQ(false, d.HasParseError());

  for (int i = 0; i < 1000000; ++i) {
    rapidjson::Value val;
    val.SetString("American", d.GetAllocator());

    rapidjson::Value::MemberIterator iter = d.FindMember("country_name");
    if (iter != d.MemberEnd()) {
      iter->value = val;
    }
  }

  rapidjson::StringBuffer buffer;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
  d.Accept(writer);
  std::string dst_json(buffer.GetString(), buffer.GetSize());
  EXPECT_EQ(dst_json, expect_json);
  fprintf(stderr, "dst_json:%s\n", dst_json.c_str());
} /*}}}*/

TEST_D(GetNthLevelKeysOfJson, Test_Normal_GetKeys_Test, "验证获取json第n层key列表") { /*{{{*/
  using namespace base;
  using namespace proto;

  std::string source_json =
      "{\"name\":\"lisi\",\"birthday\":1,\"is_student\":false,\"resv1\":\"good "
      "one\",\"resv2\":1234605616436508552,\"resv3\":-139292509886650761,\"american_friends\":[{"
      "\"id\":\"american_1\",\"name\":\"jack_1\",\"age\":21,\"addr\":{\"place\":\"New "
      "York\",\"num\":1},\"hobbies\":[{\"name\":\"swimming\",\"skill_level\":3},{\"name\":"
      "\"chess\",\"skill_level\":4},{\"name\":\"basketball\",\"skill_level\":4}]},{\"id\":"
      "\"american_2\",\"name\":\"jack_2\",\"age\":22},{\"id\":\"american_3\",\"name\":\"jack_3\","
      "\"age\":23}],\"english_friends\":[{\"id\":\"english_1\",\"name\":\"rose_1\",\"age\":21},{"
      "\"id\":\"english_2\",\"name\":\"rose_2\",\"age\":22},{\"id\":\"english_3\",\"name\":\"rose_"
      "3\",\"age\":23}],\"health_status\":101}";
  std::vector<std::string> dest_keys;
  dest_keys.push_back("id");
  dest_keys.push_back("name");
  dest_keys.push_back("age");
  dest_keys.push_back("addr");
  dest_keys.push_back("hobbies");
  dest_keys.push_back("id");
  dest_keys.push_back("name");
  dest_keys.push_back("age");
  dest_keys.push_back("id");
  dest_keys.push_back("name");
  dest_keys.push_back("age");
  dest_keys.push_back("id");
  dest_keys.push_back("name");
  dest_keys.push_back("age");
  dest_keys.push_back("id");
  dest_keys.push_back("name");
  dest_keys.push_back("age");
  dest_keys.push_back("id");
  dest_keys.push_back("name");
  dest_keys.push_back("age");

  std::vector<std::string> keys;
  Code ret = GetNthLevelKeysOfJson(source_json, 2, &keys);
  EXPECT_EQ(ret, kOk);
  EXPECT_EQ(0, CheckEqual(keys, dest_keys));

  fprintf(stderr, "keys:\n");
  for (std::vector<std::string>::iterator it = keys.begin(); it != keys.end(); ++it) {
    fprintf(stderr, "%s\n", it->c_str());
  }

} /*}}}*/

// NOTE:htt, 数据驱动测试
TEST_DATADRIVEN_D(InitJsonValue, Test_Normal_DataDrvien, "../data/json/test_init_json_new.txt",
                  const rapidjson::Value &data_case, "验证将json中指定key重新初始化") { /*{{{*/
  using namespace base;
  using namespace proto;

  EXPECT_EQ(true, data_case.IsObject());
  // NOTE:htt, 初始化字符串内容
  EXPECT_EQ(true, data_case.HasMember("src"));
  EXPECT_EQ(true, data_case["src"].IsObject());

  // NOTE:htt, 需要初始化的keys列表
  EXPECT_EQ(true, data_case.HasMember("init_keys"));
  EXPECT_EQ(true, data_case["init_keys"].IsArray());

  // NOTE:htt, 初始化函数处理返回值
  EXPECT_EQ(true, data_case.HasMember("ret"));
  EXPECT_EQ(true, data_case["ret"].IsInt());

  // NOTE:htt, 初始化成功后对应处理的字符串
  EXPECT_EQ(true, data_case.HasMember("expect"));
  EXPECT_EQ(true, data_case["expect"].IsObject());

  std::set<std::string> init_keys;
  rapidjson::Value::ConstValueIterator v_it = data_case["init_keys"].Begin();
  for (; v_it != data_case["init_keys"].End(); ++v_it) {
    EXPECT_EQ(true, v_it->IsString());

    init_keys.insert(std::string(v_it->GetString(), v_it->GetStringLength()));
  }

  std::string src_str = GetStringFromJson(data_case["src"]);
  std::string expect_str = GetStringFromJson(data_case["expect"]);

  std::string dst_str;
  int ret = InitJsonValue(src_str, init_keys, &dst_str);
  int json_ret = data_case["ret"].GetInt();
  EXPECT_EQ(json_ret, ret);
  if (json_ret == kOk) {
    EXPECT_EQ(expect_str, dst_str);
  }

  fprintf(stderr, "src_str:%s\nexpect_str:%s\ndst_str:%s\n\n", src_str.c_str(), expect_str.c_str(), dst_str.c_str());
} /*}}}*/

TEST_D(ParseFromDebugString, Test_Normal_Parse, "验证解析DebugString()内容") { /*{{{*/
  using namespace base;
  using namespace proto;

  std::string source_json =
      "{\"name\":\"lisi\",\"birthday\":1,\"is_student\":false,\"resv1\":\"good "
      "one\",\"resv2\":1234605616436508552,\"resv3\":-139292509886650761,\"american_friends\":[{"
      "\"id\":\"american_1\",\"name\":\"jack_1\",\"age\":21,\"addr\":{\"place\":\"New "
      "York\",\"num\":1},\"hobbies\":[{\"name\":\"swimming\",\"skill_level\":3},{\"name\":"
      "\"chess\",\"skill_level\":4},{\"name\":\"basketball\",\"skill_level\":4}]},{\"id\":"
      "\"american_2\",\"name\":\"jack_2\",\"age\":22},{\"id\":\"american_3\",\"name\":\"jack_3\","
      "\"age\":23}],\"english_friends\":[{\"id\":\"english_1\",\"name\":\"rose_1\",\"age\":21},{"
      "\"id\":\"english_2\",\"name\":\"rose_2\",\"age\":22},{\"id\":\"english_3\",\"name\":\"rose_"
      "3\",\"age\":23}],\"health_status\":101}";

  model::Person person;
  Code ret = proto::JsonToPBWithOutExtension(source_json, &person);
  EXPECT_EQ(kOk, ret);

  // std::string debug_str = person.DebugString();
  std::string debug_str = person.ShortDebugString();
  fprintf(stderr, "debug_str:\n%s\n", debug_str.c_str());

  model::Person check_person;
  ret = proto::ParseFromDebugString(debug_str, &check_person);
  EXPECT_EQ(ret, base::kOk);

  EXPECT_EQ(0, person.DebugString().compare(check_person.DebugString()));
  fprintf(stderr, "person.DebugString:\n%s\ncheck_peson.DebugString:\n%s\n", person.DebugString().c_str(),
          check_person.DebugString().c_str());
} /*}}}*/
