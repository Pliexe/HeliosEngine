use crate::tokens;

pub fn format_error(line: &str, token: &Token, message: &str) -> String {
    let mut error = String::new();
    error.push_str(&format!("In: {}:{}:{}\n", token.path, token.line, token.column + 1)); // cuz column starts at 1 by convetion in editors
    error.push_str(&format!("Error: {} at line {} column {}\n", message, token.line, token.column + 1));
    let line_mark = format!("{}: ", token.line);
    error.push_str(&format!("{}\n", line_mark.clone() + line));
    error.push_str(&format!("{}\n", " ".repeat(line_mark.len() + token.column) + "^".repeat(token.kind.len()).as_str()));
    error
}

pub fn format_multi_error(prev_line: &str, line: &str, prev: &Token, token: &Token, message: &str) -> String {
    let mut error = String::new();
    error.push_str(&format!("In: {}:{}:{}\n", token.path, token.line, token.column + 1));
    error.push_str(&format!("Error: {} at line {} column {}\n", message, token.line, token.column + 1));

    let line_mark = format!("{}: ", prev.line);
    error.push_str(&format!("{}\n", line_mark.clone() + prev_line));
    error.push_str(&format!("{}\n", " ".repeat(line_mark.len() + prev.column) + "^".repeat(prev.kind.len()).as_str()));

    let line_mark = format!("{}: ", token.line);
    error.push_str(&format!("{}\n", line_mark.clone() + line));
    error.push_str(&format!("{}\n", " ".repeat(line_mark.len() + token.column) + "^".repeat(token.kind.len()).as_str()));

    error
}