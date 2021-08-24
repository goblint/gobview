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
