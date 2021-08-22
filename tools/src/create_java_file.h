// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef TOOLS_CREATE_JAVA_FILE_H_
#define TOOLS_CREATE_JAVA_FILE_H_

#include <string>

#include <stdint.h>

#include "base/status.h"

namespace tools
{

const std::string kBuildGradleStr = ""
		"allprojects {\n"
		"    group = \"%s\"\n"
		"    version = '0.0.1'\n"
		"\n"
		"    repositories {\n"
		"        mavenCentral() // 即 url \"https://plugins.gradle.org/m2/\"\n"
		"    }\n"
		"}\n"
		"\n"
		"def resultType = \"\"\n"
		"def testCount = 0\n"
		"def successfulTestCount = 0\n"
		"def failedTestCount = 0\n"
		"def skippedTestCount = 0\n"
		"\n"
		"subprojects {\n"
		"    apply plugin: 'java'\n"
		"\n"
		"    // 引入测试覆盖率\n"
		"    apply from: \"$rootDir/gradle/jacoco.gradle\"\n"
		"\n"
		"    // 终端打印测试信息\n"
		"    test {\n"
		"        testLogging {\n"
		"            showStandardStreams = true\n"
		"            exceptionFormat 'full'\n"
		"            events 'started', 'passed', 'skipped', 'failed'\n"
		"        }\n"
		"\n"
		"        afterSuite { desc, result ->\n"
		"            if (!desc.parent) {\n"
		"                println(\"${result.resultType} \" +\n"
		"                        \"(${result.testCount} tests, \" +\n"
		"                        \"${result.successfulTestCount} successes, \" +\n"
		"                        \"${result.failedTestCount} failures, \" +\n"
		"                        \"${result.skippedTestCount} skipped)\")\n"
		"\n"
		"                resultType = result.resultType\n"
		"                testCount += result.testCount\n"
		"                successfulTestCount += result.successfulTestCount\n"
		"                failedTestCount += result.failedTestCount\n"
		"                skippedTestCount += result.skippedTestCount\n"
		"\n"
		"                println(\"Whole Test: ${resultType} \" +\n"
		"                        \"(${testCount} tests, \" +\n"
		"                        \"${successfulTestCount} successes, \" +\n"
		"                        \"${failedTestCount} failures, \" +\n"
		"                        \"${skippedTestCount} skipped)\")\n"
		"            }\n"
		"        }\n"
		"    }\n"
		"}\n";


const std::string kJacocoStr = ""
        "//Using legacy plugin application\n"
        "buildscript {\n"
        "    repositories {\n"
        "        mavenCentral()  // 即 url \"https://plugins.gradle.org/m2/\"\n"
        "    }\n"
        "}\n"
        "\n"
        "apply plugin: 'jacoco'\n"
        "jacoco{\n"
        "    toolVersion = \"0.8.4\"\n"
        "    reportsDir = file(\"$buildDir/jacoco\")\n"
        "}\n";

const std::string kDependsOnStr = "    dependsOn { project(':%s').jar }\n";
const std::string kFromStr =      "    from project(':%s').libsDir\n";
const std::string kDistributionGradleStr = ""
		"apply plugin: 'distribution'\n"
		"\n"
		"// task 用来复制build出来的主jar包\n"
		"task copyLibs(type: Copy) {\n"
		"    // 当前模块的编译并非有序，复制前需确认对应模块的jar先构建\n"
		"%s"
		"\n"
		"    // 将需要项目的jar复制到当前文件\n"
		"%s"
		"    into(libsDir)\n"
		"}\n"
		"\n"
		"// 把上述的task串联起来\n"
		"task prepareFile(dependsOn: [\n"
		"        'copyLibs'\n"
		"]){}\n"
		"\n"
		"distributions {\n"
		"    main {\n"
		"        baseName = rootProject.name\n"
		"\n"
		"        contents {\n"
		"            from(libsDir) {\n"
		"                exclude project.name // 去除当前模块的jar，因为仅仅是为了打包\n"
		"            }\n"
		"        }\n"
		"    }\n"
		"}\n"
        "\n"
        "distTar.dependsOn 'prepareFile'\n"
        "distTar.extension = 'tar.gz'\n"
        "distZip.dependsOn 'prepareFile'\n";

const std::string kSubProjectBuildGradleStr = ""
		"dependencies {\n"
		"    testCompile \"junit:junit:4.11\"\n"
		"}\n";

const std::string kJavaSrcStr = ""
		"package %s;\n"
		"\n"
		"/**\n"
		" ** hello world \n"
		" **\n"
		" **/\n"
		"public class App\n"
		"{\n"
		"    public int sum(int num1, int num2) {\n"
		"        return num1+num2;\n"
		"    }\n"
		"\n"
		"    public static void main( String[] args )\n"
		"    {\n"
		"        System.out.println(\"Hello World!\");\n"
		"    }\n"
		"}\n";


const std::string kJavaTestStr = ""
        "package %s;\n"
        "\n"
        "import static org.junit.Assert.assertEquals;\n"
        "import static org.junit.Assert.assertTrue;\n"
        "\n"
        "import org.junit.Test;\n"
        "\n"
        "\n"
        "/**\n"
        " ** Unit test for App\n"
        " **\n"
        " **/\n"
        "\n"
        "public class AppTest\n"
        "{\n"
        "    /**\n"
        "     * * Rigorous Test :-)\n"
        "     **/\n"
        "    @Test\n"
        "    public void shouldAnswerWithTrue()\n"
        "    {\n"
        "        assertTrue( true );\n"
        "    }\n"
        "\n"
        "    @Test\n"
        "    public void testSum()\n"
        "    {\n"
        "        App app = new App();\n"
        "        int num1 = 1;\n"
        "        int num2 = 4;\n"
        "        int sum = app.sum(num1, num2);\n"
        "\n"
        "        assertEquals(5, sum);\n"
        "    }\n"
        "}\n";

const std::string kGradleWrapper = "cd %s; gradle wrapper --gradle-version 4.5 --distribution-type all";

base::Code CreateJavaFile(const std::string &project_name, const std::string &package, const std::string &modules);

}

#endif
