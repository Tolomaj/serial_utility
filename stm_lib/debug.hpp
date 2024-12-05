
//! v případě změny interface měňtě pouze následující 2 makra
#define GENERIC_PRINTF(...) printf(__VA_ARGS__); // univerzální funkce pro výpis
#define GENERIC_NL() printf("\n");               // univerzální funkce pro nový řádek
#define SHOW_FILE_PATH true
#define SHOW_PRIORITY true

#if SHOW_PRIORITY
    #define PRINT_HEDER_PRIORITY(PRIORITY) GENERIC_PRINTF(PRIORITY);
#else
    #define PRINT_HEDER_PRIORITY(PRIORITY)
#endif

#if SHOW_FILE_PATH
    #define PRINT_HEADER_PATH() GENERIC_PRINTF("[%s:%d]",   __FILE__ , __LINE__);
#else
    #define PRINT_HEADER_PATH()
#endif

//pomocné makra
#define TEST_HEADER(x)

// print 
#define E_PRINTF(...) PRINT_HEDER_PRIORITY(); PRINT_HEADER_PATH(); GENERIC_PRINTF("[ERROR][%s:%d]",   __FILE__ , __LINE__); GENERIC_PRINTF(__VA_ARGS__ ); GENERIC_NL();
#define S_PRINTF(...) PRINT_HEDER_PRIORITY(); PRINT_HEADER_PATH(); GENERIC_PRINTF("[SILENT][%s:%d]",  __FILE__ , __LINE__); GENERIC_PRINTF(__VA_ARGS__ ); GENERIC_NL();
#define I_PRINTF(...) PRINT_HEDER_PRIORITY(); PRINT_HEADER_PATH(); GENERIC_PRINTF("[INFO][%s:%d]",    __FILE__ , __LINE__); GENERIC_PRINTF(__VA_ARGS__ ); GENERIC_NL();
#define D_PRINTF(...) PRINT_HEDER_PRIORITY(); PRINT_HEADER_PATH(); GENERIC_PRINTF("[DEBUG][%s:%d]",   __FILE__ , __LINE__); GENERIC_PRINTF(__VA_ARGS__ ); GENERIC_NL();
#define W_PRINTF(...) PRINT_HEDER_PRIORITY(); PRINT_HEADER_PATH(); GENERIC_PRINTF("[WARNING][%s:%d]", __FILE__ , __LINE__); GENERIC_PRINTF(__VA_ARGS__ ); GENERIC_NL();
#define K_PRINTF(...) PRINT_HEDER_PRIORITY(); PRINT_HEADER_PATH(); GENERIC_PRINTF("[OK][%s:%d]",      __FILE__ , __LINE__); GENERIC_PRINTF(__VA_ARGS__ ); GENERIC_NL();
#ifdef PRINTF(...)
   #define PRINTF(...)  GENERIC_PRINTF("[NORMAL][%s:%d]",   __FILE__ , __LINE__); GENERIC_PRINTF(__VA_ARGS__ ); GENERIC_NL(); 
#endif

// pouze test s generickou hláškou o úspěchu / neuspěchu
#define TEST(x)         if(x == true){  GENERIC_PRINTF("[TEST_OK][%s:%d]",__FILE__ , __LINE__); }else{  GENERIC_PRINTF("[TEST_NOK][%s:%d]",__FILE__ , __LINE__); };  GENERIC_NL();
//test který zároveň vypíše i chybovou hlášku
#define TESTF(x, ... )  if(x == true){  GENERIC_PRINTF("[TEST_OK][%s:%d]",__FILE__ , __LINE__); }else{  GENERIC_PRINTF("[TEST_NOK][%s:%d]",__FILE__ , __LINE__); }; GENERIC_PRINTF(__VA_ARGS__ ); GENERIC_NL();

//todo přidat soft breakpointy
//#define SOFT_BREAKPOINT(...)

//breakpoint neinteraguje s serial_utility je potřeba jlink k odblokování
#define BREAKPOINT() __asm__ __volatile__("BKPT");

