use std::fmt;

use crate::tokens::preprocessorkind::PreprocessorKind;
use crate::tokens::operatorkind::OperatorKind;

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

#[derive(Debug, Clone)]
pub struct Token {
    pub kind: TokenKind,
    pub global_line: usize,
    pub line: usize,
    pub column: usize,
    pub path: String,
}

impl Token {
    pub fn new(kind: TokenKind, global_line: usize, line: usize, column: usize, path: String) -> Token {
        Token {
            kind,
            global_line,
            line,
            column,
            path,
        }
    }

    pub fn to_string(&self) -> String {
        self.kind.to_string()
    }

    pub fn len(&self) -> usize {
        self.kind.len()
    }
}

pub struct TokenIter<'a> {
    pub(crate) iter: std::iter::Peekable<std::slice::Iter<'a, Token>>,
    pub(crate) current: Option<&'a Token>,
    original: Vec<&'a str>,
}

impl<'a> TokenIter<'a> {
    pub fn new(iter: std::slice::Iter<'a, Token>, original: &'a str) -> TokenIter<'a> {
        TokenIter { iter: iter.peekable(), current: None, original: original.split_terminator('\n').collect() }
    }

    pub fn get_line(&self, line: usize) -> Option<&'a str> {
        if line > self.original.len() {
            return None;
        }

        Some(self.original[line - 1])
    }

    pub fn get_current_line(&self) -> Option<&'a str> {
        self.get_line(self.current.unwrap().line)
    }

    pub fn next(&mut self) -> Option<&'a Token> {
        self.iter.next().and_then(|token| {
            self.current = Some(token);
            Some(token)
        })
    }

    pub fn skip_while(&mut self, kind: TokenKind) -> Option<&'a Token> {
        let mut token = None;
        
        while self.iter.peek().is_some() && self.iter.peek().unwrap().kind == kind {
            token = self.next();
        }

        token
    }

    pub fn collect_while(&mut self, condition: impl Fn(&Token) -> bool) -> Vec<&'a Token> {
        let mut tokens = Vec::new();
        
        while self.iter.peek().is_some() && condition(self.iter.peek().unwrap()) {
            tokens.push(self.next().unwrap());
        }

        tokens
    }

    pub fn collect_string_while(&mut self, condition: impl Fn(&Token) -> bool) -> String {
        let mut string = String::new();

        while self.iter.peek().is_some() && condition(self.iter.peek().unwrap()) {
            string.push_str(&self.next().unwrap().to_string());
        }

        string
    }

    #[allow(unused)]
    pub fn peek(&mut self) -> Option<&&'a Token> {
        self.iter.peek()
    }

    #[allow(unused)]
    pub fn expect_next_token(&mut self, error_expected: Option<&str>) -> Result<&'a Token, String>
    {
        let token = self.next().ok_or(format!("Expected {}, found end of file", error_expected.unwrap_or("token")))?;

        Ok(token)
    }
        
    pub fn expect_identifier(&mut self, expected: Option<&str>, error_expected: Option<&str>) -> Result<&'a Token, String> {
        let token = self.iter.peek().ok_or(format!("Expected {}, found end of file", error_expected.unwrap_or("identifier")))?;
        
        match &token.kind {
            TokenKind::Identifier(s) => {
                if expected.is_some() {
                    let unwrap = expected.unwrap();
                    if s == unwrap {
                        Ok(self.next().unwrap())
                    } else {
                        Err(format_error(self.original[token.global_line - 1], token, format!("Expected {} but found {}", unwrap, s).as_str()))
                    }
                } else {
                    Ok(self.next().unwrap())
                }
            },
            _ => Err(format_error(self.original[token.global_line - 1], token, format!("Expected {}, found {}", error_expected.unwrap_or("identifier"), token.kind).as_str()))
        }
    }

    pub fn expect_oldbody(&mut self, error_expected: Option<&str>) -> Result<&'a Token, String> {
        let token = self.iter.peek().ok_or("Expected body but found end of file".to_string())?;
        
        match &token.kind {
            TokenKind::OldCodeBlock(_) => { Ok(self.next().unwrap()) },
            _ => Err(format_error(self.original[token.global_line - 1], token, format!("Expected {}, found {}", error_expected.unwrap_or("body"), token.kind).as_str())),
        }
    }

    pub fn expect_body(&mut self, error_expected: Option<&str>) -> Result<(&'a Token, &'a Vec<Token>), String> {
        let token = self.iter.peek().ok_or("Expected body but found end of file".to_string())?;
        
        match &token.kind {
            TokenKind::CodeBlock(body) => { Ok((self.next().unwrap(), body)) },
            _ => Err(format_error(self.original[token.global_line - 1], token, format!("Expected {}, found {}", error_expected.unwrap_or("body"), token.kind).as_str())),
        }
    }

    pub fn expect_whitespace(&mut self, error_expected: Option<&str>) -> Result<&'a Token, String> {
        let token = self.iter.peek().ok_or("Expected whitespace but found end of file".to_string())?;
        
        match &token.kind {
            TokenKind::Whitespace(_) => { Ok(self.next().unwrap()) },
            _ => Err(format_error(self.original[token.global_line - 1], token, format!("Expected {}, found {}", error_expected.unwrap_or("whitespace"), token.kind).as_str())),
        }
    }

    pub fn expect_binding_marker(&mut self, error_expected: Option<&str>) -> Result<&'a Token, String> {
        let token = self.iter.peek().ok_or("Expected binding marker but found end of file".to_string())?;
        
        match &token.kind {
            TokenKind::Binding => { Ok(self.next().unwrap()) },
            _ => Err(format_error(self.original[token.global_line - 1], token, format!("Expected {}, found {}", error_expected.unwrap_or("binding marker"), token.kind).as_str())),
        }
    }

    pub fn expect_space_marker(&mut self, error_expected: Option<&str>) -> Result<&'a Token, String> {
        let token = self.iter.peek().ok_or("Expected space marker but found end of file".to_string())?;
        
        match &token.kind {
            TokenKind::Space => { Ok(self.next().unwrap()) },
            _ => Err(format_error(self.original[token.global_line - 1], token, format!("Expected {}, found {}", error_expected.unwrap_or("space marker"), token.kind).as_str())),
        }
    }

    pub fn expect_import_marker(&mut self, error_expected: Option<&str>) -> Result<&'a Token, String> {
        let token = self.iter.peek().ok_or("Expected import marker but found end of file".to_string())?;
        
        match &token.kind {
            TokenKind::Import => { Ok(self.next().unwrap()) },
            _ => Err(format_error(self.original[token.global_line - 1], token, format!("Expected {}, found {}", error_expected.unwrap_or("import marker"), token.kind).as_str())),
        }
    }

    pub fn expect_implements_marker(&mut self, error_expected: Option<&str>) -> Result<&'a Token, String> {
        let token = self.iter.peek().ok_or("Expected implements marker but found end of file".to_string())?;
        
        match &token.kind {
            TokenKind::Implements => { Ok(self.next().unwrap()) },
            _ => Err(format_error(self.original[token.global_line - 1], token, format!("Expected {}, found {}", error_expected.unwrap_or("implements marker"), token.kind).as_str())),
        }
    }

    pub fn expect_keyword(&mut self, expected: Option<&str>, error_expected: Option<&str>) -> Result<&'a Token, String> {
        let token = self.iter.peek().ok_or("Expected keyword but found end of file".to_string())?;
        
        match &token.kind {
            TokenKind::Keyword(_) => {
                if expected.is_some() {
                    let unwrap = expected.unwrap();
                    if token.kind == TokenKind::Keyword(unwrap.to_string()) {
                        Ok(self.next().unwrap())
                    } else {
                        Err(format_error(self.original[token.global_line - 1], token, format!("Expected {} but found {}", error_expected.unwrap_or(format!("'{}'", unwrap).as_str()), token.kind).as_str()))
                    }
                } else {
                    Ok(self.next().unwrap())
                }
            },
            _ => Err(format_error(self.original[token.global_line - 1], token, format!("Expected {}, found {}", error_expected.unwrap_or(expected.and_then(|c| Some(format!("'{}'", c))).unwrap_or("keyword".to_string()).as_str()), format!("'{}'", token.kind)).as_str())),
        }
    }

    pub fn expect_literal(&mut self, expected: Option<&str>, error_expected: Option<&str>) -> Result<&'a Token, String> {
        let token = self.iter.peek().ok_or("Expected \"\" but found end of file".to_string())?;
        
        match &token.kind {
            TokenKind::Literal(_) => {
                if expected.is_some() {
                    let unwrap = expected.unwrap();
                    if token.kind == TokenKind::Import {
                        Ok(self.next().unwrap())
                    } else {
                        Err(format_error(self.original[token.global_line - 1], token, format!("Expected {} but found {}", error_expected.unwrap_or(format!("'{}'", unwrap).as_str()), token.kind).as_str()))
                    }
                } else {
                    Ok(self.next().unwrap())
                }
            },
            _ => Err(format_error(self.original[token.global_line - 1], token, format!("Expected {}, found {}", error_expected.unwrap_or(expected.and_then(|c| Some(format!("'{}'", c))).unwrap_or("\"\"".to_string()).as_str()), format!("'{}'", token.kind)).as_str())),
        }
    }

    pub fn expect_operator(&mut self, expected: Option<OperatorKind>, error_expected: Option<&str>) -> Result<&'a Token, String> {
        let token = self.iter.peek().ok_or("Expected operator but found end of file".to_string())?;
        
        match &token.kind {
            TokenKind::Operator(_) => {
                if expected.is_some() {
                    let unwrap = expected.unwrap();
                    if token.kind == TokenKind::Operator(unwrap.clone()) {
                        Ok(self.next().unwrap())
                    } else {
                        Err(format_error(self.original[token.global_line - 1], token, format!("Expected {} but found {}", error_expected.unwrap_or(format!("'{}'", unwrap.to_string()).as_str()), token.kind).as_str()))
                    }
                } else {
                    Ok(self.next().unwrap())
                }
            },
            _ => Err(format_error(self.original[token.global_line - 1], token, format!("Expected {}, found {}", error_expected.unwrap_or(expected.and_then(|c| Some(format!("'{}'", c.clone().to_str()))).unwrap_or("\"\"".to_string()).as_str()), format!("'{}'", token.kind)).as_str())),
        }
    }

    pub fn expect_integer(&mut self, expected: Option<&str>, error_expected: Option<&str>) -> Result<&'a Token, String> {
        let token = self.iter.peek().ok_or("Expected \"\" but found end of file".to_string())?;
        
        match &token.kind {
            TokenKind::Integer(_) => {
                if expected.is_some() {
                    let unwrap = expected.unwrap();
                    if token.kind == TokenKind::Integer(unwrap.parse().unwrap()) {
                        Ok(self.next().unwrap())
                    } else {
                        Err(format_error(self.original[token.global_line - 1], token, format!("Expected {} but found {}", error_expected.unwrap_or(format!("'{}'", unwrap).as_str()), token.kind).as_str()))
                    }
                } else {
                    Ok(self.next().unwrap())
                }
            },
            _ => Err(format_error(self.original[token.global_line - 1], token, format!("Expected {}, found {}", error_expected.unwrap_or(expected.and_then(|c| Some(format!("'{}'", c))).unwrap_or("\"\"".to_string()).as_str()), format!("'{}'", token.kind)).as_str())),
        }
    }

    pub fn expect_pontosveso(&mut self) -> Result<&'a Token, String> {
        let token = self.iter.peek().ok_or("Expected ';' but found end of file".to_string())?;

        match &token.kind {
            TokenKind::Terminator(';') => {
                Ok(self.skip_while(TokenKind::Terminator(';')).unwrap())
            },
            _ => Err(format_error(self.original[token.global_line - 1], token, "Expected ';', found {}")),
        }
    }

    pub fn expect_terminator(&mut self, expected: Option<char>, error_expected: Option<&str>) -> Result<&'a Token, String> {
        let token = self.iter.peek().ok_or("Expected terminator but found end of file".to_string())?;

        match &token.kind {
            TokenKind::Terminator(_) => {
                if expected.is_some() {
                    let unwrap = expected.unwrap();
                    if token.kind == TokenKind::Terminator(unwrap) {
                        Ok(self.next().unwrap())
                    } else {
                        let line = self.original[token.global_line - 1];

                        if self.current.is_some() && token.line != self.current.unwrap().line
                        {
                            let prev_line = self.original[self.current.unwrap().global_line - 1];
                            Err(format_multi_error(prev_line, line, self.current.unwrap(), token, format!("Expected {} but found {}", error_expected.unwrap_or(format!("'{}'", unwrap).replace('\n', "\\n").as_str()), token.kind).as_str()))
                        } else {
                            Err(format_error(line, token, format!("Expected {} but found {}", error_expected.unwrap_or(format!("'{}'", unwrap).as_str()), token.kind).as_str()))
                        }

                    }
                } else {
                    Ok(self.next().unwrap())
                }
            },
            _ => {
                let line = self.original[token.global_line - 1];

                if self.current.is_some() && token.line != self.current.unwrap().line
                {
                    let prev_line = self.original[self.current.unwrap().global_line - 1];
                    Err(format_multi_error(prev_line, line, self.current.unwrap(), token, format!("Expected {}, found {}", error_expected.unwrap_or(expected.and_then(|c| Some(format!("'{}'", c))).unwrap_or("terminator".to_string()).replace('\n', "\\n").as_str()), format!("'{}'", token.kind)).as_str()))
                } else {
                    Err(format_error(line, token, format!("Expected {}, found {}", error_expected.unwrap_or(expected.and_then(|c| Some(format!("'{}'", c))).unwrap_or("terminator".to_string()).as_str()), format!("'{}'", token.kind)).as_str()))
                }
            }
        }
    }

    pub fn expect_symbol(&mut self, expected: Option<char>, error_expected: Option<&str>) -> Result<&'a Token, String> {
        let token = self.iter.peek().ok_or("Expected symbol but found end of file".to_string())?;
        
        match &token.kind {
            TokenKind::Symbol(_) => {
                if expected.is_some() {
                    let unwrap = expected.unwrap();
                    if token.kind == TokenKind::Symbol(unwrap) {
                        Ok(self.next().unwrap())
                    } else {
                        Err(format_error(self.original[token.global_line - 1], token, format!("Expected {} but found {}", error_expected.unwrap_or(format!("'{}'", unwrap).as_str()), token.kind).as_str()))
                    }
                } else {
                    Ok(self.next().unwrap())
                }
            },
            _ => Err(format_error(self.original[token.global_line - 1], token, format!("Expected {}, found {}", error_expected.unwrap_or(expected.and_then(|c| Some(format!("'{}'", c))).unwrap_or("symbol".to_string()).as_str()), format!("'{}'", token.kind)).as_str())),
        }
    }
}


#[derive(Debug, Clone)]
pub enum TokenKind {
    Keyword(String),
    Identifier(String),
    OldCodeBlock(String),
    CodeBlock(Vec<Token>),
    Literal(String),
    Integer(String),

    Preprocessor(PreprocessorKind),
    Import,
    Implements,
    Export,
    Binding,
    Type,
    Space,

    // For macro preprocessor only
    Whitespace(String),
    LineComment(String),
    BlockComment(String),

    NoRequiredDefineStart,
    NoRequiredDefineEnd,

    Symbol(char),
    Operator(OperatorKind),
    Terminator(char),

    Unknown(String),
}

impl TokenKind {
    pub fn to_string(&self) -> String {
        match self {
            TokenKind::Keyword(s) => s.clone(),
            TokenKind::Identifier(s) => s.clone(),
            TokenKind::CodeBlock(tokens) => {
                let mut s = String::new();
                for token in tokens {
                    match token.kind {
                        TokenKind::Terminator(';') => s.push_str(";\n"),
                        _ => s.push_str(&(token.to_string() + " ")),
                    }
                }
                s
            },
            TokenKind::OldCodeBlock(s) => s.clone(),
            TokenKind::Literal(s) => s.clone(),
            TokenKind::Integer(s) => s.clone(),
            TokenKind::Preprocessor(s) => s.to_string().clone(),
            TokenKind::Import => "import".to_string(),
            TokenKind::Implements => "implements".to_string(),
            TokenKind::Export => "export".to_string(),
            TokenKind::Binding => "binding".to_string(),
            TokenKind::Type => "type".to_string(),
            TokenKind::Space => " ".to_string(),
            TokenKind::Whitespace(s) => s.clone(),
            TokenKind::LineComment(s) => s.clone(),
            TokenKind::BlockComment(s) => s.clone(),
            TokenKind::NoRequiredDefineStart => "@no_required_define_start".to_string(),
            TokenKind::NoRequiredDefineEnd => "@no_required_define_end".to_string(),
            TokenKind::Operator(s) => s.to_string(),
            TokenKind::Symbol(c) | TokenKind::Terminator(c) =>
            {
                c.to_string()
            },
            TokenKind::Unknown(s) => s.clone(),
        }
    }

    pub fn len(&self) -> usize {
        match self {
            TokenKind::Keyword(s) => s.len(),
            TokenKind::Identifier(s) => s.len(),
            TokenKind::OldCodeBlock(s) => s.len(),
            TokenKind::CodeBlock(tokens) => {
                let mut len = 0;
                for token in tokens {
                    if token.kind == TokenKind::Terminator(';') {
                        len += 2;
                    } else {
                        len += token.len() + 1;
                    }
                }
                len
            },
            TokenKind::Literal(s) => s.len(),
            TokenKind::Integer(s) => s.len(),
            TokenKind::Preprocessor(s) => s.length(),
            TokenKind::Import => "@import".len(),
            TokenKind::Implements => "@implements".len(),
            TokenKind::Export => "@export".len(),
            TokenKind::Binding => "@binding".len(),
            TokenKind::Type => "@type".len(),
            TokenKind::Space => " ".len(),
            TokenKind::Whitespace(s) => s.len(),
            TokenKind::LineComment(s) => s.len(),
            TokenKind::BlockComment(s) => s.len(),
            TokenKind::NoRequiredDefineStart => "@no_required_define_start".len(),
            TokenKind::NoRequiredDefineEnd => "@no_required_define_end".len(),
            TokenKind::Symbol(_) | TokenKind::Operator(_) | TokenKind::Terminator(_) => 1,
            TokenKind::Unknown(s) => s.len(),
        }
    }

    pub fn variant(&self) -> &'static str {
        match self {
            TokenKind::Keyword(_) => "Keyword",
            TokenKind::Identifier(_) => "Identifier",
            TokenKind::OldCodeBlock(_) => "CodeBlock",
            TokenKind::CodeBlock(_) => "CodeBlock",
            TokenKind::Literal(_) => "Literal",
            TokenKind::Integer(_) => "Integer",
            TokenKind::Preprocessor(_) => "Preprocessor",
            TokenKind::Import => "Import",
            TokenKind::Implements => "Implements",
            TokenKind::Export => "Export",
            TokenKind::Binding => "Binding",
            TokenKind::Type => "Type",
            TokenKind::Space => "Space",
            TokenKind::Whitespace(_) => "Whitespace",
            TokenKind::LineComment(_) => "LineComment",
            TokenKind::BlockComment(_) => "BlockComment",
            TokenKind::NoRequiredDefineStart => "NoRequiredDefineStart",
            TokenKind::NoRequiredDefineEnd => "NoRequiredDefineEnd",
            TokenKind::Symbol(_) => "Symbol",
            TokenKind::Operator(_) => "Operator",
            TokenKind::Terminator(_) => "Terminator",
            TokenKind::Unknown(_) => "Unknown",
        }
    }
}

impl PartialEq for TokenKind 
{
    fn eq(&self, other: &Self) -> bool {
        match (self, other) {
            (TokenKind::Keyword(s1), TokenKind::Keyword(s2)) => s1 == s2,
            (TokenKind::Identifier(s1), TokenKind::Identifier(s2)) => s1 == s2,
            (TokenKind::OldCodeBlock(s1), TokenKind::OldCodeBlock(s2)) => s1 == s2,
            (TokenKind::Literal(s1), TokenKind::Literal(s2)) => s1 == s2,
            (TokenKind::Integer(s1), TokenKind::Integer(s2)) => s1 == s2,
            (TokenKind::Preprocessor(s1), TokenKind::Preprocessor(s2)) => s1 == s2,
            (TokenKind::Import, TokenKind::Import) => true,
            (TokenKind::Implements, TokenKind::Implements) => true,
            (TokenKind::Export, TokenKind::Export) => true,
            (TokenKind::Binding, TokenKind::Binding) => true,
            (TokenKind::Type, TokenKind::Type) => true,
            (TokenKind::Space, TokenKind::Space) => true,
            (TokenKind::Whitespace(s1), TokenKind::Whitespace(s2)) => s1 == s2,
            (TokenKind::LineComment(s1), TokenKind::LineComment(s2)) => s1 == s2,
            (TokenKind::BlockComment(s1), TokenKind::BlockComment(s2)) => s1 == s2,
            (TokenKind::NoRequiredDefineStart, TokenKind::NoRequiredDefineStart) => true,
            (TokenKind::NoRequiredDefineEnd, TokenKind::NoRequiredDefineEnd) => true,
            (TokenKind::Symbol(c1), TokenKind::Symbol(c2)) => c1 == c2,
            (TokenKind::Operator(c1), TokenKind::Operator(c2)) => c1 == c2,
            (TokenKind::Terminator(c1), TokenKind::Terminator(c2)) => c1 == c2,
            _ => false,
        }
    }
}

impl fmt::Display for TokenKind {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        f.write_str(self.to_string().as_str())
    }
}