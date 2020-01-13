fn main() {
    println!("cargo:rustc-link-search=native=/home/sunflower/dev/q/aardvark/rust/externals/quickjs/");
    println!("cargo:rustc-link-lib=static=quickjs");
}
