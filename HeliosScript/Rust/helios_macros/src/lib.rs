
use proc_macro::TokenStream;
use quote::{format_ident, quote, ToTokens};
use syn::{parse::Parser, parse_macro_input, Field, ItemStruct};
// static HELIOS_API_FUNCTIONS: Mutex<Vec<String>> = Mutex::new(Vec::new());

// #[macro_use]
// extern crate lazy_static;

// lazy_static! {
//     static ref HELIOS_API_FUNCTIONS: Mutex<Vec<String>> = Mutex::new(Vec::new());
// }

#[proc_macro_derive(EntityBehaviour)]
pub fn entity_behaviour(item: TokenStream) -> TokenStream {
    let input = parse_macro_input!(item as ItemStruct);
    println!("input: {}", input.to_token_stream().to_string());

    // // input.fields.named.push(parse_quote! { pub fn on_awake(&mut self) {} });

    // if let syn::Fields::Named(ref mut fields) = input.fields {
    //     fields.named.push(parse_quote! { pub fn on_awake(&mut self) {} });
    // }

    // // quote! {
    // //     #input
    // //     fn my_method(&self) {
    // //         println!("Hello, World!");
    // //     }
    // // }.into()

    // let fields: Vec<syn::Field> = vec![];

    // // input.fields.

    // // Fields::from(fields);

    // // input.fields = input.fields.

    // let name = input.ident.clone();

    let expanded = quote! {
        // #name {

        // }
        // #input

        pub fn test_add() { println!("Hello, World! from test_add"); }
    };

    println!("expanded: {}", expanded.to_token_stream().to_string());

    TokenStream::from(expanded)
}

#[proc_macro_attribute]
pub fn helios_api(args: TokenStream, item: TokenStream) -> proc_macro::TokenStream {
    let _ = args;
    let input = parse_macro_input!(item as syn::Item);

    match input {
        syn::Item::Struct(item_struct) => {
            let name = &item_struct.ident;
            let c_fn_name = format_ident!("helios_struct_create_{}", name);
            let d_fn_name = format_ident!("helios_struct_destroy_{}", name);
            // Check if base=behaviour is set
            let base = if args.to_string().contains("base=behaviour") {
                quote! { u32 test; }
            } else {
                quote! {}
            };

            println!("base: {}", base.to_token_stream().to_string());

            // let base
            let init_default: bool = args.to_string().contains("init");

            println!("struct::: {}", item_struct.to_token_stream().to_string());
            println!("c_fn_name: {}", c_fn_name);

            let struct_additional_attrs = if init_default {
                quote! { #[derive(Default)] }
            } else {
                quote! {}
            };

            let init_struct = if init_default {
                quote! {
                    { ..Default::default()}
                }
            } else {
                quote! { { } }
            };

            let mut mutated_struct = item_struct.clone();

            // match &mut mutated_struct.fields {
            //     syn::Fields::Named(fields) => {
            //         fields.named.push(syn::Field::parse_named.parse2(quote! { test: String }).unwrap());
            //     }
            //     _ => panic!("Only named fields are supported"),
            // }

            println!("mutated_struct: {}", mutated_struct.to_token_stream().to_string());

            if args.to_string().contains("base=behaviour") {
                if let syn::Fields::Named(ref mut fields) = mutated_struct.fields {
                    fields
                        .named.insert(0, Field::parse_named.parse2(quote! { test: u32 }).unwrap());
                };
            }



            // let struct_fields = item_struct.fields.iter();

            // let fields = quote! {
            //     test: u32,
            //     #(#struct_fields)*
            // };

            // println!("fields: {}", quote! {
            //     #struct_additional_attrs
            //     struct #name {
            //         #fields
            //     }
            // }.to_token_stream().to_string());

            // item_struct.fields = syn::Fields::from(parse_quote! {
            //     #(item_struct#fields)*
            // });

            quote! {
                #struct_additional_attrs
                // #visibility struct #(#name)#generics {
                //     #fields
                // }
                #mutated_struct

                #[no_mangle]
                pub extern "C" fn #c_fn_name() -> *mut #name {
                    // let mut data: #name = ;
                    Box::into_raw(Box::new(#name #init_struct))
                }

                #[no_mangle]
                pub extern "C" fn #d_fn_name(ptr: *mut #name) {
                    unsafe {
                        drop(Box::from_raw(ptr));
                    }
                }
            }
        }
        syn::Item::Fn(item_fn) => {
            println!("item_fn: {}", item_fn.to_token_stream().to_string());
            // HELIOS_API_FUNCTIONS
            //     .lock()
            //     .unwrap()
            //     .push(item_fn.sig.ident.to_string());
            let name = format_ident!("helios_func_{}", item_fn.sig.ident);

            // let mut file =
            //     File::create("function_names.bin").expect("Unable to create file(); Hello, World!");

            // let mut bytes = Vec::new();
            // for function_name in helios_globals::HELIOS_API_FUNCTIONS.lock().unwrap().iter() {
            //     bytes.extend_from_slice(function_name.as_bytes());
            // }

            // if !bytes.is_empty() {
            //     bytes.push(0);
            // }

            // let byte_array = helios_globals::HELIOS_API_FUNCTIONS
            //     .lock()
            //     .unwrap()
            //     .join("\n")
            //     .as_bytes()
            //     .to_vec();
            // // writeln!(file, "{}", name).expect("Unable to write to file");
            // // file.write_all(helios_globals::HELIOS_API_FUNCTIONS.lock().unwrap().map).expect("Unable to write to file");
            // file.write_all(&bytes).expect("Unable to write to file");
            // // // writeln!()
            // // file.sync_data().unwrap();
            // // file.close().unwrap();

            // // writeln!(file, "{}", name).expect("Unable to write to file").unwrap();

            let body = item_fn.block;
            let mut sig = item_fn.sig.clone();
            println!("SIG: {}", sig.to_token_stream().to_string());
            sig.ident = name;
            let attr = item_fn.attrs;
            quote! {
                #(#attr)*
                #[no_mangle]
                pub extern "C" #sig {
                    #body
                }

                // HELIOS_API_FUNCTIONS.lock().unwrap().push(item_fn.sig.ident.to_string());
            }
        }
        _ => unimplemented!("`helios_api` attribute. Only functions are supported"),
    }
    .into()
}
