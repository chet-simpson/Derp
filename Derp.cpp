#include "QABasicParser.h"

/*
In this context widgets represents the primary skill, expertise, or xxx of the interviewee.
Say I hand you the final design for a small set of 5 or 6 classes.  The design defines
the public interface, behavior of the public interface for each class.

AS a developer what steps do you take
(or processes do you follow) to provide a production ready implementation?

Define class invariants.
exception safety



How many ways are there to use new and delete? This should include new/delete, new[]/delete[].
and placement new.
*/

/*
    Token           75
    Lexer           300
    TokenSequence   250
    Diagnostics     75
*/



int main()
{
    /*
        Script COULD define

            form factors
            scaling factors
            click areas (pick selection)
            button definitions (click areas with special attributes)
            device definitions (form factor, tests, attributes, etc)
    */
    static const std::wstring script1 =
        L"testcase Empty\n"
        L"begin\n"
        L"end\n"
        ;

    static const std::wstring script2 =
        L"testcase Commands_General\n"
        L"begin\n"
        L"\tverify BaseWinAmount is 0.\n"
        L"\tload \"RedBaron\".\n"
        L"\tset balance 10000\n"
        L"\tset max bet.\n"
        L"\tset max line.\n"
        L"\tset bet index 1.\n"
        L"\tset line index 4.\n"
        L"\tclick screen 1 1.\n"
        L"\tclick Feature1.\n"
        L"\ttrigger feature 2.\n"
        L"\tincrease bet\n"
        L"\tincrease line\n"
        L"\tdecrease bet\n"
        L"\tdecrease line\n"
        L"\tenable sound\n"
        L"\tdisable sound.\n"
        L"\tinsert credit.\n"
        L"\tinsert 5 credits.\n"
        L"\tspin reels.\n"
        L"end\n"
        ;
        //

    static const std::wstring script3 =
        L"testcase Example1\n"
        L"begin\n"
        L"\twait 7500ms\n"               //  Needs BNF
        L"\twait for \"Playthrough\" event\n"               //  Needs BNF
        L"\twait for \"GameLoad\" event\n"               //  Needs BNF
        L"\twait for GameLoad event.\n"     //  Needs BNF
        L"\twait for SpinComplete event\n"                 //  Needs BNF
        L"\twait 10000 ms for SpinComplete event.\n"   //  Needs BNF
        L"\twait 5000ms for SpinComplete event .\n"     //  Needs BNF
        L"\twait for FeatureComplete event\n"     //  Needs BNF
        L"\twait 5000ms for SpinComplete event .\n"     //  Needs BNF
        L"\twait for SpinResult event.\n"     //  Needs BNF
        L"end\n"
        ;

    static const std::wstring script4 =
        L"testcase Example2\n"
        L"params\n"
        L"  $gamename\n"
        L"  $featureTestcase\n"
        L"begin\n"
        L"\tload $gamename.\n"
        L"\trun Variables3.\n"
        L"\trun $featureTestcase.\n"
        L"\trun Variables1[10000, 5, 10, 10, \"buttonname\", 2].\n"
        L"\trun $featureTestcase[1000, 2500, \"FeaturePick\", \"BaseWinAmount\", 25000].\n"
        L"\texit game.\n"
        L"end\n"
        ;

    static const std::wstring script5 =
        L"testcase Parameters1\n"
        L"params\n"
        L"  $params1\n"
        L"  $params2\n"
        L"  $params3\n"
        L"begin\n"
        L"end\n"
        ;

    static const std::wstring script6 =
        L"testcase ParametersAsCommandArguments1\n"
        L"params\n"
        L"  $balance\n"
        L"  $index\n"
        L"  $x\n"
        L"  $y\n"
        L"  $label\n"
        L"  $featureid\n"
        L"begin\n"
        L"\tset balance $balance\n"
        L"\tset bet index $index.\n"
        L"\tset line index $index.\n"
        L"\tclick screen $x $y.\n"
        L"\tclick $label.\n"
        L"\ttrigger feature $featureid.\n"
        L"end\n"
        ;

    static const std::wstring script7 =
        L"testcase ParametersAsCommandArguments2\n"
        L"params\n"
        L"  $amount\n"
        L"  $timevalue\n"
        L"  $eventname\n"
        L"  $attrname\n"
        L"  $basewin\n"
        L"begin\n"
        L"\tinsert $amount credits.\n"
        L"\twait $timevalue ms for $eventname event.\n"   //  Needs BNF
        L"\tverify $attrname is $basewin.\n"
        L"end\n"
        ;


    static const std::wstring scriptTemplate =
        L"testcase Template\n"
        L"begin\n"
        L"end\n"
        ;

    static const std::wstring scriptX =
        L"testcase Template\n"
        L"begin\n"
        L"\n"
        L"end\n"
        ;


    Derp::ErrorReporting  reporting;
    Lexer lexer;

    for (auto script : {
        //script1, script2, script3, script4,
        //script5, script6, script7,
        scriptX
    })
    {
        std::wcout << L"--------------------------\nParsing:\n" << script << std::endl << std::endl;

        auto nodes = Parser(lexer, reporting).Parse(script);
        for (auto& node : nodes)
        {
            node->SerializeOut(std::wcout, 0);
            std::wcout << std::endl;
        }
    }
}


/*

    #####################################################################################
    #   Standard test cases applied to all new media games.
    #####################################################################################
    testcase NewMedia_Standard_ReelPower {}         #   10 test cases for all reel power games

    testcase NewMedia_Standard {}                   #   ***** Runs the following test cases
    testcase NewMedia_Standard_Level_1_Gaffes {}    #   6 test cases tailored to exercise gaffes applicable to all games.
    testcase NewMedia_Standard_Level_2_Gaffes {}    #   4 test cases tailored to exercise gaffes applicable to all games.
    testcase NewMedia_Standard_Level_3_Gaffes {}    #   8 test cases tailored to exercise gaffes applicable to all games.
    testcase NewMedia_Standard_Level_4_Gaffes {}    #   5 test cases tailored to exercise gaffes applicable to all games.

    testcase NewMedia_RedBaron {}                   #   ***** Runs the following test cases
    testcase NewMedia_RedBaron_Level_1_Gaffes       #   4 testcases tailored to exercise gaffes applicable only to red baron.
    {
        run NewMedia_Standard_Level_1_Gaffes
        # new gaffes
    }
    testcase NewMedia_RedBaron_TestSet_1 {}         #   8 testcases tailored to red baron.
    testcase NewMedia_RedBaron_TestSet_2 {}         #   18 testcases tailored to red baron.
    testcase NewMedia_RedBaron_TestSet_3 {}         #   3 testcases tailored to red baron.
    testcase NewMedia_RedBaron_TestSet_4 {}         #   5 testcases tailored to red baron.
    testcase NewMedia_RedBaron_TestSet_5 {}         #   7 testcases tailored to red baron.
    testcase NewMedia_RedBaron_TestSet_6 {}         #   7 testcases tailored to red baron.
    testcase NewMedia_RedBaron_TestSet_7 {}         #   2 testcases tailored to red baron.


    game RedBaron
    {
        run NewMedia_Standard_ReelPower
        run NewMedia_Standard
        run NewMedia_RedBaron
    }



    game_decl: `game` IDENTIFIER EOL `begin` EOL (game_attribute EOL)* `end` ;

    game_attribute:
         `set` IDENTIFIER `to` (IDENTIFIER | VARAIBLE | STRING | INTEGER | NUMBER)

        utilize the name of the feature reported by the feature scene (ReportFeatureBegin) through the lobby wrapper callback interface

*/