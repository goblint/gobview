//Provides: unix_getpid
function unix_getpid() {
    return 0;
}

//Provides: unix_kill
function unix_kill(pid, signal) { }

//Provides: unix_setitimer
function unix_setitimer(which, newval) {
    return newval;
}

//Provides: ml_z_mul_overflows
function ml_z_mul_overflows(x) {
    // This is necessary until https://github.com/janestreet/zarith_stubs_js/pull/8 is fixed.
    return true;
}

//Provides: camlidl_apron_init
function camlidl_apron_init() { return 0; }

//Provides: camlidl_apron_set_var_operations
function camlidl_apron_set_var_operations() { return 0; }

//Provides: camlidl_oct_oct_manager_alloc
function camlidl_oct_oct_manager_alloc() { return 0; }

//Provides: camlidl_polka_pk_manager_alloc_loose
function camlidl_polka_pk_manager_alloc_loose() { return 0; }

//Provides: camlidl_box_box_manager_alloc
function camlidl_box_box_manager_alloc() { return 0; }

//Provides: camlidl_var_ap_var_of_string
function camlidl_var_ap_var_of_string() { return 0; }

//Provides: camlidl_environment_ap_environment_make
function camlidl_environment_ap_environment_make() { return 0; }

//Provides: camlidl_abstract1_ap_abstract1_top
function camlidl_abstract1_ap_abstract1_top() { return 0; }

//Provides: camlidl_var_ap_var_hash
function camlidl_var_ap_var_hash() { return 0; }
