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

//Provides: unix_times
//only used for timing statistics in Goblint
function unix_times(x) {
  return 0;
}

//Provides: max_float
//from Goblint specific stubs for C float operations
function max_float(x) {
  return Number.MAX_VALUE;
}

//Provides: smallest_float
//from Goblint specific stubs for C float operations
//returns the smallest positive and normalized float
function smallest_float(x) {
  return 2.2250738585072014e-308;
}

//Provides: ml_z_of_int
//Requires: ml_z_of_int64, ml_z_to_int64
function ml_z_of_int(x) {
    return ml_z_of_int64(ml_z_to_int64(x));
}

//Provides: caml_thread_initialize const
function caml_thread_initialize() {}

//Provides: caml_mutex_new const
function caml_mutex_new() {}

//Provides: camlidl_apron_init
function camlidl_apron_init() { return 0; }

//Provides: camlidl_apron_set_var_operations
function camlidl_apron_set_var_operations() { return 0; }

//Provides: camlidl_oct_oct_manager_alloc
function camlidl_oct_oct_manager_alloc() { return 0; }

//Provides: camlidl_polka_pk_manager_alloc_loose
function camlidl_polka_pk_manager_alloc_loose() { return 0; }

//Provides: camlidl_polka_pk_manager_alloc_equalities
function camlidl_polka_pk_manager_alloc_equalities() { return 0; }

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

//Provides: camlidl_mpq_mpq_init
function camlidl_mpq_mpq_init() { return 0; }

//Provides: camlidl_mpq_mpq_set_si
function camlidl_mpq_mpq_set_si() { return 0; }
