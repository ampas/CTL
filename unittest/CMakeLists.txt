add_subdirectory(IlmCtl)
add_subdirectory(IlmCtlMath)
add_subdirectory(IlmImfCtl)
if (CTL_BUILD_TOOLS)
    add_subdirectory(ctlrender)
    add_subdirectory(exrdpx)
    add_subdirectory(exr_ctl_exr)
endif()

