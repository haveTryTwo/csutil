// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <string>
#include <sstream>

#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>

#include <google/protobuf/message.h>

#include "proto/pb_to_json.h"

#include "test_base/include/test_base.h"

#include "pb_src/model.pb.h"

TEST(PBToJsonWithOutExtension, Test_Normal_Person)
{/*{{{*/
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

    std::string expect_json = "{\"name\":\"zhangsan\",\"age\":100,\"country_name\":\"China\",\"birthday\":1,\"height\":1.73,\"weight\":105.41300201416016,\"is_student\":false,\"resv1\":\"good one\",\"resv2\":1234605616436508552,\"resv3\":-139292509886650761}";
    EXPECT_EQ(expect_json, json);

    // fprintf(stderr, "pb_str:\n%s\n", person.DebugString().c_str());
    // fprintf(stderr, "size:%zu, json:       %s\n", json.size(), json.c_str());
    // fprintf(stderr, "size:%zu, expect_json:%s\n", expect_json.size(), expect_json.c_str());
}/*}}}*/

TEST(PBToJsonWithOutExtension, Test_Normal_Person_Enum)
{/*{{{*/
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

    std::string expect_json = "{\"name\":\"zhangsan\",\"birthday\":1,\"is_student\":false,\"resv1\":\"good one\",\"resv2\":1234605616436508552,\"resv3\":-139292509886650761,\"health_status\":102}";
    EXPECT_EQ(expect_json, json);

    // fprintf(stderr, "pb_str:\n%s\n", person.DebugString().c_str());
    // fprintf(stderr, "size:%zu, json:       %s\n", json.size(), json.c_str());
    // fprintf(stderr, "size:%zu, expect_json:%s\n", expect_json.size(), expect_json.c_str());
}/*}}}*/

TEST(PBToJsonWithOutExtension, Test_Normal_Person_UpperField)
{/*{{{*/
    using namespace base;

    model::Person person; 
    person.set_name("zhangsan");
    person.set_birthday(1);
    person.set_is_student(false);
    person.set_resv1("good one");
    person.set_resv2(0x1122334455667788);
    person.set_resv3(0xfe11223344556677);
    person.set_resv4(0xfe11223344556677); // Note: upper field will be set of RESV4 
    person.set_health_status(model::HS_MID);

    std::string json;
    Code ret = proto::PBToJsonWithOutExtension(person, &json);
    EXPECT_EQ(kOk, ret);

    std::string expect_json = "{\"name\":\"zhangsan\",\"birthday\":1,\"is_student\":false,\"resv1\":\"good one\",\"resv2\":1234605616436508552,\"resv3\":-139292509886650761,\"RESV4\":-139292509886650761,\"health_status\":102}";
    EXPECT_EQ(expect_json, json);

    // fprintf(stderr, "pb_str:\n%s\n", person.DebugString().c_str());
    // fprintf(stderr, "size:%zu, json:       %s\n", json.size(), json.c_str());
    // fprintf(stderr, "size:%zu, expect_json:%s\n", expect_json.size(), expect_json.c_str());
}/*}}}*/

TEST(PBToJsonWithOutExtension, Test_Normal_Person_Repeated_String)
{/*{{{*/
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

    std::string expect_json = "{\"name\":\"lisi\",\"birthday\":1,\"is_student\":false,\"resv1\":\"good one\",\"resv2\":1234605616436508552,\"resv3\":-139292509886650761,\"resv11\":[17,34,51,68],\"friends\":[\"Test1\",\"Test2\",\"Test3\",\"Test4\"],\"health_status\":101}";
    EXPECT_EQ(expect_json, json);

    // fprintf(stderr, "pb_str:\n%s\n", person.DebugString().c_str());
    // fprintf(stderr, "size:%zu, json:       %s\n", json.size(), json.c_str());
    // fprintf(stderr, "size:%zu, expect_json:%s\n", expect_json.size(), expect_json.c_str());
}/*}}}*/

TEST(PBToJsonWithOutExtension, Test_Normal_Person_Repeated_OtherType)
{/*{{{*/
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

    std::string expect_json = "{\"name\":\"lisi\",\"birthday\":1,\"is_student\":false,\"resv1\":\"good one\",\"resv2\":1234605616436508552,\"resv3\":-139292509886650761,\"resv11\":[17,34,51,68],\"resv12\":[11.11,22.11,33.11,44.11],\"resv13\":[false,true,true,false],\"resv14\":[\"adfas23243\",\"*lasdfnklwe\",\"热爱祖国\",\"好好学习，天天向上！！！\"],\"resv15\":[101,102,103,101],\"health_status\":101}";
    EXPECT_EQ(expect_json, json);

    // fprintf(stderr, "pb_str:\n%s\n", person.DebugString().c_str());
    // fprintf(stderr, "size:%zu, json:       %s\n", json.size(), json.c_str());
    // fprintf(stderr, "size:%zu, expect_json:%s\n", expect_json.size(), expect_json.c_str());
}/*}}}*/

TEST(PBToJsonWithOutExtension, Test_Normal_Person_Repeated_Object)
{/*{{{*/
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

    std::string expect_json = "{\"name\":\"lisi\",\"birthday\":1,\"is_student\":false,\"resv1\":\"good one\",\"resv2\":1234605616436508552,\"resv3\":-139292509886650761,\"american_friends\":[{\"id\":\"american_1\",\"name\":\"jack_1\",\"age\":21,\"addr\":{\"place\":\"New York\",\"num\":1},\"hobbies\":[{\"name\":\"swimming\",\"skill_level\":3},{\"name\":\"chess\",\"skill_level\":4},{\"name\":\"basketball\",\"skill_level\":4}]},{\"id\":\"american_2\",\"name\":\"jack_2\",\"age\":22},{\"id\":\"american_3\",\"name\":\"jack_3\",\"age\":23}],\"english_friends\":[{\"id\":\"english_1\",\"name\":\"rose_1\",\"age\":21},{\"id\":\"english_2\",\"name\":\"rose_2\",\"age\":22},{\"id\":\"english_3\",\"name\":\"rose_3\",\"age\":23}],\"health_status\":101}";
    EXPECT_EQ(expect_json, json);

    // fprintf(stderr, "pb_str:\n%s\n", person.DebugString().c_str());
    // fprintf(stderr, "size:%zu, json:       %s\n", json.size(), json.c_str());
    // fprintf(stderr, "size:%zu, expect_json:%s\n", expect_json.size(), expect_json.c_str());
}/*}}}*/

TEST(PBToJsonWithOutExtension, Test_Normal_Person_Object)
{/*{{{*/
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
    std::string expect_json = "{\"name\":\"lisi\",\"birthday\":1,\"is_student\":false,\"resv1\":\"good one\",\"resv2\":1234605616436508552,\"resv3\":-139292509886650761,\"addrs\":{\"place\":\"广东深圳\",\"num\":1},\"health_status\":101}";
    EXPECT_EQ(expect_json, json);

    // fprintf(stderr, "pb_str:\n%s\n", person.DebugString().c_str());
    // fprintf(stderr, "size:%zu, json:       %s\n", json.size(), json.c_str());
    // fprintf(stderr, "size:%zu, expect_json:%s\n", expect_json.size(), expect_json.c_str());
}/*}}}*/

TEST(PBToJsonWithOutExtension, Test_Normal_Person_Extension)
{/*{{{*/
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

    std::string expect_json = "{\"name\":\"lisi\",\"birthday\":1,\"is_student\":false,\"resv1\":\"good one\",\"resv2\":1234605616436508552,\"resv3\":-139292509886650761,\"addrs\":{\"place\":\"广东深圳\",\"num\":1},\"health_status\":101}";
    EXPECT_EQ(expect_json, json);
    // fprintf(stderr, "pb_str:\n%s\n", person.DebugString().c_str());
    // fprintf(stderr, "size:%zu, json:       %s\n", json.size(), json.c_str());
    // fprintf(stderr, "size:%zu, expect_json:%s\n", expect_json.size(), expect_json.c_str());
    
}/*}}}*/

TEST(PBToJsonWithOutExtension, Test_Normal_Person_Empty_Name)
{/*{{{*/
    using namespace base;

    model::Person person; 

    std::string json;
    Code ret = proto::PBToJsonWithOutExtension(person, &json);
    EXPECT_EQ(kOk, ret);

    std::string expect_json = "{\"name\":\"\"}"; // Note: Person.name is required
    EXPECT_EQ(expect_json, json);
    // fprintf(stderr, "pb_str:\n%s\n", person.DebugString().c_str());
    // fprintf(stderr, "size:%zu, json:       %s\n", json.size(), json.c_str());
    // fprintf(stderr, "size:%zu, expect_json:%s\n", expect_json.size(), expect_json.c_str());
    
}/*}}}*/

TEST(PBToJsonWithOutExtension, Test_Press_Person_Ten_Thousand)
{/*{{{*/
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
    for (int i = 0; i < 10000; ++i)
    {
        json.clear();
        Code ret = proto::PBToJsonWithOutExtension(person, &json);
        EXPECT_EQ(kOk, ret);
    }

    std::string expect_json = "{\"name\":\"lisi\",\"birthday\":1,\"is_student\":false,\"resv1\":\"good one\",\"resv2\":1234605616436508552,\"resv3\":-139292509886650761,\"american_friends\":[{\"id\":\"american_1\",\"name\":\"jack_1\",\"age\":21,\"addr\":{\"place\":\"New York\",\"num\":1},\"hobbies\":[{\"name\":\"swimming\",\"skill_level\":3},{\"name\":\"chess\",\"skill_level\":4},{\"name\":\"basketball\",\"skill_level\":4}]},{\"id\":\"american_2\",\"name\":\"jack_2\",\"age\":22},{\"id\":\"american_3\",\"name\":\"jack_3\",\"age\":23}],\"english_friends\":[{\"id\":\"english_1\",\"name\":\"rose_1\",\"age\":21},{\"id\":\"english_2\",\"name\":\"rose_2\",\"age\":22},{\"id\":\"english_3\",\"name\":\"rose_3\",\"age\":23}],\"health_status\":101}";
    EXPECT_EQ(expect_json, json);

    // fprintf(stderr, "pb_str:\n%s\n", person.DebugString().c_str());
    // fprintf(stderr, "size:%zu, json:       %s\n", json.size(), json.c_str());
    // fprintf(stderr, "size:%zu, expect_json:%s\n", expect_json.size(), expect_json.c_str());
}/*}}}*/


TEST(JsonToPBWithOutExtension, Test_Normal_Person)
{/*{{{*/
    using namespace base;

    std::string source_json = "{\"name\":\"zhangsan\",\"age\":100,\"country_name\":\"China\",\"birthday\":1,\"height\":1.73,\"weight\":105.41300201416016,\"is_student\":false,\"resv1\":\"good one\",\"resv2\":1234605616436508552,\"resv3\":-139292509886650761}";

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
}/*}}}*/

TEST(JsonToPBWithOutExtension, Test_Normal_Person_Enum)
{/*{{{*/
    using namespace base;

    std::string source_json = "{\"name\":\"zhangsan\",\"birthday\":1,\"is_student\":false,\"resv1\":\"good one\",\"resv2\":1234605616436508552,\"resv3\":-139292509886650761,\"health_status\":102}";

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
}/*}}}*/

TEST(JsonToPBWithOutExtension, Test_Normal_Person_UpperField)
{/*{{{*/
    using namespace base;

    std::string source_json = "{\"name\":\"zhangsan\",\"birthday\":1,\"is_student\":false,\"resv1\":\"good one\",\"resv2\":1234605616436508552,\"resv3\":-139292509886650761,\"resv4\":-139292509886650761,\"health_status\":102}";

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
    dest_person.set_resv4(0xfe11223344556677); // Note: upper field will be set of RESV4 
    dest_person.set_health_status(model::HS_MID);

    EXPECT_EQ(dest_person.DebugString(), person.DebugString());
}/*}}}*/

TEST(JsonToPBWithOutExtension, Test_Normal_Person_Repeated_String)
{/*{{{*/
    using namespace base;

    std::string source_json = "{\"name\":\"lisi\",\"birthday\":1,\"is_student\":false,\"resv1\":\"good one\",\"resv2\":1234605616436508552,\"resv3\":-139292509886650761,\"resv11\":[17,34,51,68],\"friends\":[\"Test1\",\"Test2\",\"Test3\",\"Test4\"],\"health_status\":101}";

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
}/*}}}*/

TEST(JsonToPBWithOutExtension, Test_Normal_Person_Repeated_OtherType)
{/*{{{*/
    using namespace base;

    std::string source_json = "{\"name\":\"lisi\",\"birthday\":1,\"is_student\":false,\"resv1\":\"good one\",\"resv2\":1234605616436508552,\"resv3\":-139292509886650761,\"resv11\":[17,34,51,68],\"resv12\":[11.11,22.11,33.11,44.11],\"resv13\":[false,true,true,false],\"resv14\":[\"adfas23243\",\"*lasdfnklwe\",\"热爱祖国\",\"好好学习，天天向上！！！\"],\"resv15\":[101,102,103,101],\"health_status\":101}";

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
}/*}}}*/

TEST(JsonToPBWithOutExtension, Test_Normal_Person_Repeated_Object)
{/*{{{*/
    using namespace base;

    std::string source_json = "{\"name\":\"lisi\",\"birthday\":1,\"is_student\":false,\"resv1\":\"good one\",\"resv2\":1234605616436508552,\"resv3\":-139292509886650761,\"american_friends\":[{\"id\":\"american_1\",\"name\":\"jack_1\",\"age\":21,\"addr\":{\"place\":\"New York\",\"num\":1},\"hobbies\":[{\"name\":\"swimming\",\"skill_level\":3},{\"name\":\"chess\",\"skill_level\":4},{\"name\":\"basketball\",\"skill_level\":4}]},{\"id\":\"american_2\",\"name\":\"jack_2\",\"age\":22},{\"id\":\"american_3\",\"name\":\"jack_3\",\"age\":23}],\"english_friends\":[{\"id\":\"english_1\",\"name\":\"rose_1\",\"age\":21},{\"id\":\"english_2\",\"name\":\"rose_2\",\"age\":22},{\"id\":\"english_3\",\"name\":\"rose_3\",\"age\":23}],\"health_status\":101}";

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
}/*}}}*/

TEST(JsonToPBWithOutExtension, Test_Normal_Person_Object)
{/*{{{*/
    using namespace base;
    std::string source_json = "{\"name\":\"lisi\",\"birthday\":1,\"is_student\":false,\"resv1\":\"good one\",\"resv2\":1234605616436508552,\"resv3\":-139292509886650761,\"addrs\":{\"place\":\"广东深圳\",\"num\":1},\"health_status\":101}";

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
}/*}}}*/

TEST(JsonToPBWithOutExtension, Test_Normal_Person_Extension)
{/*{{{*/
    using namespace base;

    std::string source_json = "{\"name\":\"lisi\",\"birthday\":1,\"is_student\":false,\"resv1\":\"good one\",\"resv2\":1234605616436508552,\"resv3\":-139292509886650761,\"addrs\":{\"place\":\"广东深圳\",\"num\":1},\"health_status\":101}";

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
}/*}}}*/

TEST(JsonToPBWithOutExtension, Test_Normal_Person_Empty_Name)
{/*{{{*/
    using namespace base;

    std::string source_json = "{\"name\":\"\"}"; // Note: Person.name is required

    model::Person person; 
    Code ret = proto::JsonToPBWithOutExtension(source_json, &person);
    EXPECT_EQ(kOk, ret);

    // fprintf(stderr, "pb_str:\n%s\n", person.DebugString().c_str());
    // fprintf(stderr, "size:%zu, json:       %s\n", source_json.size(), source_json.c_str());

    model::Person dest_person; 
    dest_person.set_name("");

    EXPECT_EQ(dest_person.DebugString(), person.DebugString());
}/*}}}*/

TEST(JsonToPBWithOutExtension, Test_Normal_Person_StringEnum)
{/*{{{*/
    using namespace base;

    std::string source_json = "{\"name\":\"zhangsan\",\"birthday\":1,\"is_student\":false,\"resv1\":\"good one\",\"resv2\":1234605616436508552,\"resv3\":-139292509886650761,\"health_status\":\"HS_MID\"}";

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
}/*}}}*/

TEST(JsonToPBWithOutExtension, Test_Press_Person_Ten_Thousand)
{/*{{{*/
    using namespace base;

    std::string source_json = "{\"name\":\"lisi\",\"birthday\":1,\"is_student\":false,\"resv1\":\"good one\",\"resv2\":1234605616436508552,\"resv3\":-139292509886650761,\"american_friends\":[{\"id\":\"american_1\",\"name\":\"jack_1\",\"age\":21,\"addr\":{\"place\":\"New York\",\"num\":1},\"hobbies\":[{\"name\":\"swimming\",\"skill_level\":3},{\"name\":\"chess\",\"skill_level\":4},{\"name\":\"basketball\",\"skill_level\":4}]},{\"id\":\"american_2\",\"name\":\"jack_2\",\"age\":22},{\"id\":\"american_3\",\"name\":\"jack_3\",\"age\":23}],\"english_friends\":[{\"id\":\"english_1\",\"name\":\"rose_1\",\"age\":21},{\"id\":\"english_2\",\"name\":\"rose_2\",\"age\":22},{\"id\":\"english_3\",\"name\":\"rose_3\",\"age\":23}],\"health_status\":101}";

    model::Person person; 
    for (int i = 0; i < 10000; ++i)
    {
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
}/*}}}*/

TEST(JsonToPBWithOutExtension, Test_Exception_Person_InvalidNumEnum)
{/*{{{*/
    using namespace base;

    std::string source_json = "{\"name\":\"zhangsan\",\"birthday\":1,\"is_student\":false,\"resv1\":\"good one\",\"resv2\":1234605616436508552,\"resv3\":-139292509886650761,\"health_status\":111111}";

    model::Person person; 
    Code ret = proto::JsonToPBWithOutExtension(source_json, &person);
    EXPECT_EQ(base::kInvalidParam, ret);
}/*}}}*/

TEST(JsonToPBWithOutExtension, Test_Exception_Person_InvalidStringEnum)
{/*{{{*/
    using namespace base;

    std::string source_json = "{\"name\":\"zhangsan\",\"birthday\":1,\"is_student\":false,\"resv1\":\"good one\",\"resv2\":1234605616436508552,\"resv3\":-139292509886650761,\"health_status\":\"JUST_ERROR\"}";

    model::Person person; 
    Code ret = proto::JsonToPBWithOutExtension(source_json, &person);
    EXPECT_EQ(base::kInvalidParam, ret);
}/*}}}*/

TEST(JsonToPBWithOutExtension, Test_Exception_Person_InvalidTypeEnum)
{/*{{{*/
    using namespace base;

    std::string source_json = "{\"name\":\"zhangsan\",\"birthday\":1,\"is_student\":false,\"resv1\":\"good one\",\"resv2\":1234605616436508552,\"resv3\":-139292509886650761,\"health_status\":true}";

    model::Person person; 
    Code ret = proto::JsonToPBWithOutExtension(source_json, &person);
    EXPECT_EQ(base::kInvalidParam, ret);
}/*}}}*/

TEST(JsonToPBWithOutExtension, Test_Exception_Person_Repeated_InvalidNumEnum)
{/*{{{*/
    using namespace base;

    std::string source_json = "{\"name\":\"lisi\",\"birthday\":1,\"is_student\":false,\"resv1\":\"good one\",\"resv2\":1234605616436508552,\"resv3\":-139292509886650761,\"resv11\":[17,34,51,68],\"resv12\":[11.11,22.11,33.11,44.11],\"resv13\":[false,true,true,false],\"resv14\":[\"adfas23243\",\"*lasdfnklwe\",\"热爱祖国\",\"好好学习，天天向上！！！\"],\"resv15\":[1111111,102,103,101],\"health_status\":101}";

    model::Person person; 
    Code ret = proto::JsonToPBWithOutExtension(source_json, &person);
    EXPECT_EQ(base::kInvalidParam, ret);
}/*}}}*/

TEST(JsonToPBWithOutExtension, Test_Exception_Person_Repeated_InvalidStringEnum)
{/*{{{*/
    using namespace base;

    std::string source_json = "{\"name\":\"lisi\",\"birthday\":1,\"is_student\":false,\"resv1\":\"good one\",\"resv2\":1234605616436508552,\"resv3\":-139292509886650761,\"resv11\":[17,34,51,68],\"resv12\":[11.11,22.11,33.11,44.11],\"resv13\":[false,true,true,false],\"resv14\":[\"adfas23243\",\"*lasdfnklwe\",\"热爱祖国\",\"好好学习，天天向上！！！\"],\"resv15\":[\"iiiijjj\"],\"health_status\":101}";

    model::Person person; 
    Code ret = proto::JsonToPBWithOutExtension(source_json, &person);
    EXPECT_EQ(base::kInvalidParam, ret);
}/*}}}*/

TEST(JsonToPBWithOutExtension, Test_Exception_Person_Repeated_InvalidTypeEnum)
{/*{{{*/
    using namespace base;

    std::string source_json = "{\"name\":\"lisi\",\"birthday\":1,\"is_student\":false,\"resv1\":\"good one\",\"resv2\":1234605616436508552,\"resv3\":-139292509886650761,\"resv11\":[17,34,51,68],\"resv12\":[11.11,22.11,33.11,44.11],\"resv13\":[false,true,true,false],\"resv14\":[\"adfas23243\",\"*lasdfnklwe\",\"热爱祖国\",\"好好学习，天天向上！！！\"],\"resv15\":[false, true],\"health_status\":101}";

    model::Person person; 
    Code ret = proto::JsonToPBWithOutExtension(source_json, &person);
    EXPECT_EQ(base::kInvalidParam, ret);
}/*}}}*/

TEST(JsonToPBWithOutExtension, Test_Exception_Person_InvalidIntType)
{/*{{{*/
    using namespace base;

    std::string source_json = "{\"name\":\"zhangsan\",\"birthday\":\"ERROR BIRTHDAY\",\"is_student\":false,\"resv1\":\"good one\",\"resv2\":1234605616436508552,\"resv3\":-139292509886650761,\"health_status\":111111}";

    model::Person person; 
    Code ret = proto::JsonToPBWithOutExtension(source_json, &person);
    EXPECT_EQ(base::kInvalidParam, ret);
}/*}}}*/

TEST(JsonToPBWithOutExtension, Test_Exception_Person_InvalidDoubleType)
{/*{{{*/
    using namespace base;

    std::string source_json = "{\"name\":\"zhangsan\",\"birthday\":1,\"height\":\"AAAAAA\",\"is_student\":false,\"resv1\":\"good one\",\"resv2\":1234605616436508552,\"resv3\":-139292509886650761,\"health_status\":111111}";

    model::Person person; 
    Code ret = proto::JsonToPBWithOutExtension(source_json, &person);
    EXPECT_EQ(base::kInvalidParam, ret);
}/*}}}*/

TEST(JsonToPBWithOutExtension, Test_Exception_Person_InvalidBoolType)
{/*{{{*/
    using namespace base;

    std::string source_json = "{\"name\":\"zhangsan\",\"birthday\":1,\"is_student\":1,\"resv1\":\"good one\",\"resv2\":1234605616436508552,\"resv3\":-139292509886650761,\"health_status\":111111}";

    model::Person person; 
    Code ret = proto::JsonToPBWithOutExtension(source_json, &person);
    EXPECT_EQ(base::kInvalidParam, ret);
}/*}}}*/

TEST(JsonToPBWithOutExtension, Test_Exception_Person_InvalidStringType)
{/*{{{*/
    using namespace base;

    std::string source_json = "{\"name\":true,\"birthday\":1,\"is_student\":false,\"resv1\":\"good one\",\"resv2\":1234605616436508552,\"resv3\":-139292509886650761,\"health_status\":111111}";

    model::Person person; 
    Code ret = proto::JsonToPBWithOutExtension(source_json, &person);
    EXPECT_EQ(base::kInvalidParam, ret);
}/*}}}*/

TEST(JsonToPBWithOutExtension, Test_Exception_Person_InvalidField)
{/*{{{*/
    using namespace base;

    std::string source_json = "{\"XXX\":\"zhangsan\",\"birthday\":1,\"resv1\":\"good one\",\"resv2\":1234605616436508552,\"resv3\":-139292509886650761,\"health_status\":111111}";

    model::Person person; 
    Code ret = proto::JsonToPBWithOutExtension(source_json, &person);
    EXPECT_EQ(base::kInvalidParam, ret);
}/*}}}*/

TEST(JsonToPBWithOutExtension, Test_Exception_Person_Empty)
{/*{{{*/
    using namespace base;

    std::string source_json = "";

    model::Person person; 
    Code ret = proto::JsonToPBWithOutExtension(source_json, &person);
    EXPECT_EQ(base::kInvalidParam, ret);
}/*}}}*/
