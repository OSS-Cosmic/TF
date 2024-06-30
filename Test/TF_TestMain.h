
#define TF_UTEST_MAIN(NAME)                                               \
    int main(int argc, const char* const argv[])                      \
    {                                                                 \
        FileSystemInitDesc fsDesc = {};                               \
        fsDesc.pAppName = NAME;                                       \
        if (!initMemAlloc(fsDesc.pAppName))                           \
            return EXIT_FAILURE;                                      \
        initLog(fsDesc.pAppName, DEFAULT_LOG_LEVEL);                  \
                                                                      \
        if (!initFileSystem(&fsDesc))                                 \
            return EXIT_FAILURE;                                      \
                                                                      \
        fsSetPathForResourceDir(pSystemFileIO, RM_DEBUG, RD_LOG, ""); \
                                                                      \
        int res = utest_main(argc, argv);                             \
        exitLog();                                                    \
                                                                      \
        exitMemAlloc();                                               \
                                                                      \
        exitFileSystem();                                             \
        return res;                                                   \
    }
