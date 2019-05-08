fn test_again() {
    println!("wow");
}

#[no_mangle]
extern "C" fn call_me() {
    println!("Testi!!!!");
    println!("Testi!!!!!!!");
    println!("Testi!!!!!!!!!!");
    test_again();
}
