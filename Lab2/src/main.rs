extern crate ctrlc;

use std::collections::VecDeque;
use std::env;
use std::fs::File;
use std::fs::OpenOptions;
use std::io::{stdin, stdout, Write};
use std::os::unix::io::{FromRawFd, IntoRawFd};
use std::process::{exit, Child, Command, Stdio};

fn main() {
    //use set_handler set 'ctrl + c'signal handle
    ctrlc::set_handler(move || {
        print!("\n");
        outputhead();
        stdout().flush().unwrap();
    })
    .expect("Error setting Ctrl-C handler");

    loop {
        outputhead();
        //get the input line as string
        let mut input_line = String::new();
        match stdin().read_line(&mut input_line) {
            Ok(_) => {}
            Err(err) => {
                eprintln!("An error occured: {}", err);
                continue;
            }
        }

        // peek input_line to command
        let mut commands = input_line.trim().split(" | ").peekable();
        let mut previous_command = None;

        while let Some(cmd) = commands.next() {
            let redirectreturn = redirect(cmd);

            let mut v: VecDeque<&str> = redirectreturn.0.split('$').collect();
            v.pop_front();

            let mut cmdchange = redirectreturn.0.to_owned();
            for ans in v {
                //替换环境变量
                match env::var(ans.trim()) {
                    Ok(val) => {
                        cmdchange = cmdchange.replacen("$", "", 1);
                        cmdchange = cmdchange.replacen(ans, &val, 1);
                    }
                    _ => {}
                }
            }

            let mut parts = cmdchange.trim().split_whitespace();
            let command = parts.next().unwrap();
            let mut args = parts;
            match command {
                "cd" => {
                    let dir = args.next().expect("No enough args to set current dir");
                    env::set_current_dir(dir).expect("Changing current dir failed");
                }
                "pwd" => {
                    let err = "Getting current dir failed";
                    println!("{}", env::current_dir().expect(err).to_str().expect(err));
                }
                "export" => {
                    for arg in args {
                        let mut assign = arg.split("=");
                        let name = assign.next().expect("No variable name");
                        let value = assign.next().expect("No variable value");
                        env::set_var(name, value);
                    }
                }
                "exit" => {
                    exit(0);
                }

                //let fd1 = File::create("test1").unwrap().into_raw_fd();
                command => {
                    let stdin = if redirectreturn.1 == 1 {
                        // redirection of stdin
                        let fin = File::open(redirectreturn.2).unwrap().into_raw_fd();
                        unsafe { Stdio::from_raw_fd(fin) }
                    } else {
                        previous_command.map_or(Stdio::inherit(), |output: Child| {
                            Stdio::from(output.stdout.unwrap())
                        })
                    };

                    let stdout = if commands.peek().is_some() {
                        // there is another command piped behind this one
                        // prepare to send output to the next command
                        Stdio::piped()
                    } else if redirectreturn.1 == 2 {
                        // redirection of stdout
                        let fout = File::create(redirectreturn.2).unwrap().into_raw_fd();
                        unsafe { Stdio::from_raw_fd(fout) }
                    } else if redirectreturn.1 == 3 {
                        // redirection of stdout
                        //let fout = OpenOptions::new().open(outputfile).append();
                        let fout = OpenOptions::new()
                            .append(true)
                            .open(redirectreturn.2)
                            .unwrap()
                            .into_raw_fd();
                        unsafe { Stdio::from_raw_fd(fout) }
                    } else {
                        // there are no more commands piped behind this one
                        // send output to shell stdout
                        Stdio::inherit()
                    };

                    let output = Command::new(command)
                        .args(args)
                        .stdin(stdin)
                        .stdout(stdout)
                        .spawn();

                    match output {
                        Ok(output) => {
                            previous_command = Some(output);
                        }
                        Err(e) => {
                            previous_command = None;
                            eprintln!("{}", e);
                        }
                    };
                }
            }
        }

        if let Some(mut final_command) = previous_command {
            // block until the final command has finished
            final_command.wait().unwrap();
        }
    }
}

fn outputhead() {
    //output "$usrname:$dir >" as the prompt.
    let curr_dir;
    match env::current_dir() {
        Ok(res) => curr_dir = res.as_path().display().to_string(),
        Err(_err) => curr_dir = "/".to_string(),
    }
    // 如果 curr_dir在用户home目录下 使用~/形式
    let home_dir;
    match env::var("HOME") {
        Ok(res) => home_dir = res.to_string(),
        Err(_err) => home_dir = "/".to_string(),
    }
    let show_dir = str::replace(curr_dir.as_str(), home_dir.as_str(), "~");

    let val;
    match env::var("USER") {
        Ok(res) => val = res.to_string(),
        Err(_err) => val = "couldn't interpret USER".to_string(),
    }
    print!("{}:{} $ ", val, show_dir);
    match stdout().flush() {
        Ok(_) => {}
        Err(err) => {
            eprintln!("An error occured: {}", err);
        }
    }
}

fn redirect(cmd: &str) -> (String, i32, &str) {
    //input cmd
    //output cmd before redirect signal
    //output retype
    //output in/output file
    //用于拆分"<",">>",">"
    let mut command_copy = cmd.trim().split_whitespace();
    let mut new_cmd = cmd;
    let mut file = "";
    let retype: i32;
    loop {
        if let Some(string) = command_copy.next() {
            if string == "<" {
                while let Some(next_string) = command_copy.next() {
                    file = next_string;
                    break;
                }
                new_cmd = new_cmd.split_once("<").unwrap().0;
                retype = 1;
                break;
            }
            if string == ">" {
                while let Some(next_string) = command_copy.next() {
                    file = next_string;
                    break;
                }
                new_cmd = new_cmd.split_once(">").unwrap().0;
                retype = 2;
                break;
            }
            if string == ">>" {
                while let Some(next_string) = command_copy.next() {
                    file = next_string;
                    break;
                }
                new_cmd = new_cmd.split_once(">>").unwrap().0;
                retype = 3;
                break;
            }
        } else {
            retype = 0;
            break;
        }
    }
    (new_cmd.to_string(), retype, file)
}
