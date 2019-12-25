fn main() {
    println!("cargo:rustc-link-search=native=/home/sunflower/dev/aardvark/newcore/externals/quickjs/");
    println!("cargo:rustc-link-lib=static=quickjs");
}
