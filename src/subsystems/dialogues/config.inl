// M1: exact host config/state contributions at their original callsites.
#if defined(KCD2VR_CONFIG_SAVE_DIALOGUES0)
    ok = wb("VrDialogueSmoothCamera",CameraHook::DialogueSmooth38()) && ok; // C38: menu-owned
#endif
#if defined(KCD2VR_CONFIG_LOAD_DIALOGUES0)
    CameraHook::SetDialogueSmooth38(ReadIniBoolExplicit(explicit_ini,"Settings","VrDialogueSmoothCamera",true)); // C38: missing key = ON
#endif
