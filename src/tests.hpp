#include <iostream>

void tests(Terminal * terminal){
    // multiline test
    std::string str = "[OK][main.cpp:5]helo_world!\n[~]this is folowing line 1 \n[~]this is folowing line 2 \n";
    terminal->print(str);

    // numbers test
    terminal->print("[DBG][main.cpp:5]some numbers 1234 11.22 [222]\n");

    // not a header test
    terminal->print("Some numbers 1234 11.22 [222]\n");

    // not a header test
    terminal->print("[somethingNotHeader]test\n");

    // break point test
    terminal->print("[ERR][main.cpp:15]\n");

    // break point test
    terminal->print("[ERR]\n");

    // only file test
    terminal->print("[KEdada.z:152] onyl file test\n");
    terminal->print("[main.cpp:999999999] onyl file test\n");
    terminal->print("[KEdada.z:1] onyl file test\n");
    terminal->print("[KEčda.z:1] onyl file test\n");
    terminal->print("[KEdada.z:] onyl file test\n");

    //fullpath test
    terminal->print("[C:\\Users\\ogross\\Desktop\\panel_w_editor\\src\\uproc03\\fonts.cpp:1328] fullpath test\n");
    terminal->print("[Users\\ogross\\Desktop\\panel_w_editor\\src\\uproc03\\fonts.cpp:1328] fullpath test\n");
    terminal->print("[src\\uproc03\\fonts.cpp:1328] fullpath test\n");
    terminal->print("[uproc3\\fonts.cpp:1328] fullpath test\n");
    terminal->print("[shoj/fonts.cpp:1328] fullpath test\n");
    terminal->print("[ahoj\\fonts.cpp:1328] fullpath test\n");
    terminal->print("[fonts.cpp:1328] fullpath test\n");

    // error containing
    terminal->print("some ERROR formates: ErrOr or error or ERROR or eRROR\n");

    //flag test
    terminal->print("[ERR] error\n");
    terminal->print("[E] error\n");
    terminal->print("[ERROR] error\n");

    terminal->print("[INFO] info\n");
    terminal->print("[I] info\n");

    terminal->print("[DBG] debug\n");
    terminal->print("[D] debug\n");
    terminal->print("[DEBUG] debug\n");

    terminal->print("[WARNING] warning\n");
    terminal->print("[W] warning\n");

    terminal->print("[NORMAL] normal\n");
    terminal->print("[N] normal\n");

    terminal->print("[OK] ok\n");
    terminal->print("[K] ok\n");

    terminal->print("[TEST] test\n");
    terminal->print("[T] test\n");
    
    terminal->print("there was silent messgaes V\n");
    terminal->print("[SILENT] silent\n");
    terminal->print("[S] silent\n");

    // vykřičníkové
    terminal->print("ja nemam flag err ale i tak svitim !\n");

    //big test
    terminal->print("[C:\\Users\\ogross\\Desktop\\panel_w_editor\\src\\uproc03\\fonts.cpp:1328][DBG] Kod dopadne buď OK nebo ERROR na 100% [~]. \n");

    //bbug
    terminal->print("[C:\\Users\\ogross\\Desktop\\panel_w_editor\\src\\uproc03\\fonts.cpp:1328]loading char i with w: 3 on adress 3701 with moff e52 and head 28af\n");
    
    //TESTS
    terminal->print("[TEST_OK][main.cpp:26]\n");
    terminal->print("[TEST_NOK][main.cpp:26]\n");
    terminal->print("[TEST_OK][main.cpp:26]some test text\n");
    terminal->print("[TEST_NOK][main.cpp:26]some test text\n");

    // begin tag
    terminal->print("[BEGIN] section begin\n");

    terminal->print("some text\n");

    // only type test
    terminal->print("[OK] test done\n");
}

void ftests(Terminal * terminal){
    // multiline test
    std::string str = "[OK][main.cpp:5]helo_world!\n[~]this is folowing line 1 \n[~]this is folowing line 2 \n";
    terminal->print(str);

    // only type test
    terminal->print("[OK] test done\n");
}
