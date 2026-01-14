use std::{collections::{HashMap, HashSet}, fmt, path::PathBuf};

#[derive(Debug, Clone)]
pub enum PreprocessorKind {
    Define(String),
    Undefine(String),
    Line((u32, Option<String>)),
    If(String),
    Ifdef(String),
    Ifndef(String),
    Elif(String),
    Else,
    EndIf,
    Unknown(String),
}

impl PreprocessorKind {
    pub fn to_string(&self) -> String {
        match self {
            PreprocessorKind::Define(s) => format!("#define {}", s.clone()),
            PreprocessorKind::Undefine(s) => format!("#undef {}", s.clone()),
            PreprocessorKind::Line(s) => format!("#line {} {}", s.0, s.1.clone().unwrap_or_default()).trim().to_string(),
            PreprocessorKind::If(s) => format!("#if {}", s.clone()),
            PreprocessorKind::Ifdef(s) => format!("#ifdef {}", s.clone()),
            PreprocessorKind::Ifndef(s) => format!("#ifndef {}", s.clone()),
            PreprocessorKind::Elif(s) => format!("#elif {}", s.clone()),
            PreprocessorKind::Else => "else".to_string(),
            PreprocessorKind::EndIf => "endif".to_string(),
            PreprocessorKind::Unknown(s) => s.clone(),
        }
    }

    pub fn length(&self) -> usize {
        match self {
            PreprocessorKind::Define(s) => s.len(),
            PreprocessorKind::Undefine(s) => s.len(),
            PreprocessorKind::Line(s) => (f64::log10(f64::max(1f64, s.0 as f64)) + 1.0) as usize + if s.1.is_some() { s.1.as_ref().unwrap().len() + 1 } else { 0 },
            PreprocessorKind::If(s) => s.len(),
            PreprocessorKind::Ifdef(s) => s.len(),
            PreprocessorKind::Ifndef(s) => s.len(),
            PreprocessorKind::Elif(s) => s.len(),
            PreprocessorKind::Else => 0,
            PreprocessorKind::EndIf => 0,
            PreprocessorKind::Unknown(s) => s.len(),
        }
    }
}

impl PartialEq for PreprocessorKind {
    fn eq(&self, other: &Self) -> bool {
        match (self, other) {
            (PreprocessorKind::Define(s1), PreprocessorKind::Define(s2)) => s1 == s2,
            (PreprocessorKind::Undefine(s1), PreprocessorKind::Undefine(s2)) => s1 == s2,
            (PreprocessorKind::Line(s1), PreprocessorKind::Line(s2)) => s1 == s2,
            (PreprocessorKind::If(s1), PreprocessorKind::If(s2)) => s1 == s2,
            (PreprocessorKind::Ifdef(s1), PreprocessorKind::Ifdef(s2)) => s1 == s2,
            (PreprocessorKind::Ifndef(s1), PreprocessorKind::Ifndef(s2)) => s1 == s2,
            (PreprocessorKind::Elif(s1), PreprocessorKind::Elif(s2)) => s1 == s2,
            (PreprocessorKind::Else, PreprocessorKind::Else) => true,
            (PreprocessorKind::EndIf, PreprocessorKind::EndIf) => true,
            (PreprocessorKind::Unknown(s1), PreprocessorKind::Unknown(s2)) => s1 == s2,
            _ => false,
        }
    }
}

#[derive(Debug, Clone)]
pub enum TokenKind {
    Keyword(String),
    Identifier(String),
    CodeBlock(String),
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
    Operator(char),
    Terminator(char),
}

impl TokenKind {
    pub fn to_string(&self) -> String {
        match self {
            TokenKind::Keyword(s) => s.clone(),
            TokenKind::Identifier(s) => s.clone(),
            TokenKind::CodeBlock(s) => s.clone(),
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
            TokenKind::Symbol(c) | TokenKind::Operator(c) | TokenKind::Terminator(c) =>
            {
                c.to_string()
            }
        }
    }

    pub fn length(&self) -> usize {
        match self {
            TokenKind::Keyword(s) => s.len(),
            TokenKind::Identifier(s) => s.len(),
            TokenKind::CodeBlock(s) => s.len(),
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
        }
    }

    pub fn variant(&self) -> &'static str {
        match self {
            TokenKind::Keyword(_) => "Keyword",
            TokenKind::Identifier(_) => "Identifier",
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
        }
    }
}

impl PartialEq for TokenKind 
{
    fn eq(&self, other: &Self) -> bool {
        match (self, other) {
            (TokenKind::Keyword(s1), TokenKind::Keyword(s2)) => s1 == s2,
            (TokenKind::Identifier(s1), TokenKind::Identifier(s2)) => s1 == s2,
            (TokenKind::CodeBlock(s1), TokenKind::CodeBlock(s2)) => s1 == s2,
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
        self.kind.length()
    }
}

struct TokenIter<'a> {
    iter: std::iter::Peekable<std::slice::Iter<'a, Token>>,
    current: Option<&'a Token>,
    original: Vec<&'a str>,
}

impl<'a> TokenIter<'a> {
    pub fn new(iter: std::slice::Iter<'a, Token>, original: &'a str) -> TokenIter<'a> {
        TokenIter { iter: iter.peekable(), current: None, original: original.split_terminator('\n').collect() }
    }

    pub fn next(&mut self) -> Option<&'a Token> {
        self.iter.next().and_then(|token| {
            self.current = Some(token);
            Some(token)
        })
    }

    pub fn skip_while(&mut self, kind: TokenKind) -> Option<&'a Token> {
        while self.iter.peek().is_some() && self.iter.peek().unwrap().kind == kind {
            return self.next();
        }

        None
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

    pub fn expect_body(&mut self, error_expected: Option<&str>) -> Result<&'a Token, String> {
        let token = self.iter.peek().ok_or("Expected body but found end of file".to_string())?;
        
        match &token.kind {
            TokenKind::CodeBlock(_) => { Ok(self.next().unwrap()) },
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

    pub fn expect_integer(&mut self, expected: Option<&str>, error_expected: Option<&str>) -> Result<&'a Token, String> {
        let token = self.iter.peek().ok_or("Expected \"\" but found end of file".to_string())?;
        
        match &token.kind {
            TokenKind::Integer(_) => {
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

struct CharIter<'a> {
    // input: &'a str,
    iter: std::iter::Peekable<std::str::Chars<'a>>,
    line: usize,
    column: usize,
}

impl<'a> CharIter<'a> {
    fn new(input: &'a str) -> CharIter<'a> {
        CharIter { iter: input.chars().peekable(), line: 1, column: 0 }
    }

    fn peek(&mut self) -> Option<&char> {
        self.iter.peek()
    }

    fn collect_while(&mut self, condition: fn(char) -> bool) -> String {
        let mut s = String::new();
        while let Some(c) = self.peek() {
            if condition(*c) {
                s.push(*c);
                self.next();
            } else {
                break;
            }
        }
        s
    }

    fn next_if(&mut self, condition: fn(char) -> bool) -> Option<char> {
        match self.iter.peek() {
            Some(c) => {
                if condition(*c) {
                    self.next()
                } else {
                    None
                }
            },
            None => None,
        }
    }
}

impl<'a> Iterator for CharIter<'a> {
    type Item = char;

    fn next(&mut self) -> Option<Self::Item> {
        if self.iter.peek().is_none() {
            return None;
        }

        let c = self.iter.next().unwrap();
        if c == '\n' {
            self.line += 1;
            self.column = 0;
        } else {
            self.column += 1;
        }
        Some(c)
    }
}

// inclusive
fn preprocessor_tokenizer(input: &str) -> Result<Vec<Token>, String> {
    let mut tokens: Vec<Token> = Vec::new();

    let mut char_iter = CharIter::new(input);

    let empty_string = "".to_string();

    while let Some(c) = char_iter.next() {
        match c {
            '/' => {
                match char_iter.peek() {
                    Some('/') => {
                        let orig_line = char_iter.line;
                        let orig_column = char_iter.column;

                        tokens.push(Token::new(
                            TokenKind::LineComment("/".to_string() + char_iter.collect_while(|c| c != '\n').as_str()),
                            orig_line,
                            orig_line,
                            orig_column,
                            empty_string.clone(),
                        ));
                    },
                    Some('*') => {
                        let mut depth = 1;
                        let mut comment = "/".to_string();
                        let starting_line = char_iter.line;
                        let starting_column = char_iter.column;
                        
                        while let Some(c) = char_iter.next() {

                            comment.push(c);
                            if c == '*' && char_iter.next_if(|c| c == '/').is_some() {
                                comment.push('/');
                                depth -= 1;
                            } else if c == '/' && char_iter.next_if(|c| c == '*').is_some() {
                                comment.push('*');
                                depth += 1;
                            }
                            if depth == 0 {
                                break;
                            }
                        }

                        if depth != 0 {
                            return Err(format!("Unterminated block comment from line {} column {}", starting_line, starting_column));
                        }

                        tokens.push(Token::new(
                            TokenKind::BlockComment(comment),
                            starting_line,
                            starting_line,
                            starting_column,
                            empty_string.clone(),
                        ));     
                    },
                    _ => {
                        tokens.push(Token::new(
                            TokenKind::Symbol(c),
                            char_iter.line,
                            char_iter.line,
                            char_iter.column,
                            empty_string.clone(),
                        ));
                    }
                }
            },
            '"' => {
                let mut string = '"'.to_string();
                let starting_line = char_iter.line;
                let starting_column = char_iter.column;
                
                let mut terminated = false;
                while let Some(c) = char_iter.next() {
                    string.push(c);
                    if c == '"' {
                        terminated = true;
                        break;
                    }
                }

                if !terminated {
                    return Err(format!("Unterminated string from line {} column {}", starting_line, starting_column));
                }

                tokens.push(Token::new(
                    TokenKind::Literal(string),
                    starting_line,
                    starting_line,
                    starting_column,
                    empty_string.clone(),
                ));
            },
            '\n' | ';' => {
                tokens.push(Token::new(
                    TokenKind::Terminator(c),
                    char_iter.line,
                    char_iter.line,
                    char_iter.column,
                    empty_string.clone(),
                ));
            },
            ' ' | '\t' | '\r' => {
                let whitespace = c.to_string() + char_iter.collect_while(|c| c == ' ' || c == '\t' || c == '\r').as_str();

                tokens.push(Token::new(
                    TokenKind::Whitespace(whitespace),
                    char_iter.line,
                    char_iter.line,
                    char_iter.column,
                    empty_string.clone(),
                ));
            },
            _ if c.is_ascii_alphabetic() || c == '_' => {
                let identifier = c.to_string() + char_iter.collect_while(|c| c.is_ascii_alphanumeric() || c == '_').as_str();

                tokens.push(Token::new(
                    TokenKind::Identifier(identifier),
                    char_iter.line,
                    char_iter.line,
                    char_iter.column,
                    empty_string.clone(),
                ));
            },
            _ => {
                tokens.push(Token::new(
                    TokenKind::Symbol(c),
                    char_iter.line,
                    char_iter.line,
                    char_iter.column,
                    empty_string.clone(),
                ));
            },
        }
    }

    Ok(tokens)
}


fn preprocess(original_path: &PathBuf, include_paths: &Vec<&PathBuf>) -> Result<String, String> {
    preprocess_i(original_path, include_paths, &mut Vec::new(), &mut HashMap::new())
}

fn preprocess_i(original_path: &PathBuf, include_paths: &Vec<&PathBuf>, included_files: &mut Vec<PathBuf>, defines: &mut HashMap<String, String>) -> Result<String, String> {
    let mut out = String::new();
    
    let src = std::fs::read_to_string(original_path);

    if src.is_err() {
        return Err(format!("Unable to read file: {}", src.unwrap_err()));
    }
    let src = src.unwrap();

    let tokens = preprocessor_tokenizer(src.as_str())?;

    let mut iter = TokenIter::new(tokens.iter(), src.as_str());

    let mut new_line = false;
    while let Some(token) = iter.next() {
        match &token.kind {
            TokenKind::Terminator('\n') => {
                out.push('\n');
                new_line = true;
            },
            TokenKind::Terminator('#') => {
                if !new_line {
                    out.push('#');
                    continue;
                }
                
                let _ = iter.expect_whitespace(None).and_then(|ws| {
                    out.push_str(&ws.to_string());
                    Ok(())
                });

                let macro_name = iter.expect_identifier(None, Some("macro name"))?.to_string();

                let _ = iter.expect_whitespace(None).and_then(|ws| {
                    out.push_str(&ws.to_string());
                    Ok(())
                });
                
                match macro_name.as_str() {
                    "define" => {
                        // collect body present
                    },
                    _ => {
                        return Err(format!("Unknown preprocessor directive: {}", macro_name));
                    }
                }
            }
            _ => {
                out.push_str(&token.to_string());
                new_line = false;
            }
        }
    }

    Ok(out)
}

fn tokenize(input: &str) -> Result<Vec<Token>, String> {
    let mut tokens: Vec<Token> = Vec::new();
    let mut line = 1;
    let mut line_offset = 0usize;
    let mut column = 0;
    let mut i = 0;


    let bytes = input.as_bytes();

    let mut path = "".to_string();

    let mut terminatable_line = false;

    while i < bytes.len() {
        let c = bytes[i];

        match c {
            b'\n' => {

                if terminatable_line
                {
                    terminatable_line = false;
                    tokens.push(Token::new(
                        TokenKind::Terminator('\n'),
                        line,
                        line - line_offset,
                        column + 1,
                        path.clone()
                    ));
                }

                line += 1;
                column = 0;
            },
            b' ' | b'\t' | b'\r' => {
                column += 1;
            },
            b';' => {
                column += 1;
                tokens.push(Token::new(
                    TokenKind::Terminator(c as char),
                    line,
                    line - line_offset,
                    column,
                    path.clone()
                ));
            },
            b'{' => {
                let mut indent = 1;
                let mut block = String::new();
                block.push(bytes[i] as char);
                i += 1;
                column += 1;
                while indent > 0 {
                    if i >= bytes.len() {
                        return Err("Expected closing brace but found end of file".to_string());
                    }

                    let c = bytes[i];

                    match c {
                        b'{' => indent += 1,
                        b'}' => {
                            indent -= 1;
                        },
                        b'\n' => { line += 1; column = 1; },
                        _ => {},
                    }

                    block.push(c as char);
                    column += 1;
                    i += 1;
                }
                i -= 1;
                tokens.push(Token::new(TokenKind::CodeBlock(block), line, line - line_offset, column, path.clone()));
            },
            b'+' | b'-' | b'*' | b'/' | b'%' | b'=' => {
                if i + 1 < bytes.len() && bytes[i + 1] == b'/' {
                    i += 1;
                    column = 0;
                    while i < bytes.len() && bytes[i] != b'\n' {
                        i += 1;
                    }
                    line += 1;
                } else {
                    column += 1;
                    tokens.push(Token::new(TokenKind::Operator(c as char), line, line - line_offset, column, path.clone()));
                }
            },
            b'"' => {
                let mut string = String::new();
                i += 1;
                column += 1;
                let start_column = column;
                while i < bytes.len() && bytes[i] != b'"' {
                    string.push(bytes[i] as char);
                    i += 1;
                    column += 1;
                }
                tokens.push(Token::new(TokenKind::Literal(string), line, line - line_offset, start_column, path.clone()));
            },
            b'@' => {
                if column == 0 {
                    
                    let start_column = column + 1;
                    
                    let mut identifier = String::new();
                    // identifier.push(c as char);
                    i += 1;
                    column += 1;

                    // skip until first alphabetic chracter or err just tokenize


                    if i >= bytes.len() {
                        return Err(format!("Expected identifier but found end of file at line {} column {}", line - line_offset, column));
                    }

                    let mut start = false;
                    let mut spaceoffset = 0;
                    while i < bytes.len() && (
                        bytes[i].is_ascii_alphabetic() ||
                        bytes[i].is_ascii_digit() ||
                        bytes[i] == b'_' ||
                        bytes[i] == b' ' ||
                        bytes[i] == b'\n'
                    ) {
                        if bytes[i] == b' ' {
                            if start {
                                break;
                            }

                            i += 1;
                            column += 1;
                            spaceoffset += 1;
                            continue;
                        }
                        start = true;
                        if bytes[i] == b'\n' {
                            break;
                        }
                        identifier.push(bytes[i] as char);
                        i += 1;
                        column += 1;
                    }
                    i -= 1;
                    // column += 1; // yes

                    println!("identifier: {}", identifier);
                    match identifier.as_str() {
                        "import" => { terminatable_line = true; tokens.push(Token::new(TokenKind::Import, line, line - line_offset, start_column + spaceoffset, path.clone())) },
                        "export" => { terminatable_line = true; tokens.push(Token::new(TokenKind::Export, line, line - line_offset, start_column + spaceoffset, path.clone())) },
                        "type" => { terminatable_line = true; tokens.push(Token::new(TokenKind::Type, line, line - line_offset, start_column + spaceoffset, path.clone())) },
                        "binding" => { terminatable_line = true; tokens.push(Token::new(TokenKind::Binding, line, line - line_offset, start_column + spaceoffset, path.clone())) },
                        "space" => { terminatable_line = true; tokens.push(Token::new(TokenKind::Space, line, line - line_offset, start_column + spaceoffset, path.clone())) },
                        "implements" => { terminatable_line = true; tokens.push(Token::new(TokenKind::Implements, line, line - line_offset, start_column + spaceoffset, path.clone())) },
                        "no_required_define_start" => { terminatable_line = true; tokens.push(Token::new(TokenKind::NoRequiredDefineStart, line, line - line_offset, start_column + spaceoffset, path.clone())) },
                        "no_required_define_end" => { terminatable_line = true; tokens.push(Token::new(TokenKind::NoRequiredDefineEnd, line, line - line_offset, start_column + spaceoffset, path.clone())) },
                        _ => {
                            tokens.push(Token::new(TokenKind::Symbol(c as char), line, line - line_offset, start_column + spaceoffset, path.clone()));

                            if !identifier.is_empty() {
                                tokens.push(Token::new(TokenKind::Identifier(identifier.to_string()), line, line - line_offset, start_column + spaceoffset + 1, path.clone()));
                            }
                        }
                    }

                } else {
                    tokens.push(Token::new(TokenKind::Symbol(c as char), line, line - line_offset, column, path.clone()));
                    column += 1;
                }
            },
            _ if c.is_ascii_digit() => {
                column += 1;
                let mut identifier = String::new();
                identifier.push(c as char);
                while i < bytes.len() && bytes[i+1].is_ascii_digit() {
                    identifier.push(bytes[i+1] as char);
                    i += 1;
                    column += 1;
                }
                tokens.push(Token::new(TokenKind::Integer(identifier), line, line - line_offset, column, path.clone()));
            },
            _ => {
                let mut identifier = String::new();

                let start_column = column;

                if column == 0 && c == b'#' {
                    terminatable_line = true;
                    i += 1;
                    while i < bytes.len() && (bytes[i].is_ascii_alphabetic() || bytes[i] == b'_' || bytes[i].is_ascii_digit()) {
                        identifier.push(bytes[i] as char);
                        i += 1;
                        column += 1;
                    }
                    if identifier.is_empty() {
                        tokens.push(Token::new(TokenKind::Symbol(c as char), line, line - line_offset, column, path.clone()));
                        column += 1;
                    } else {

                        if i + 1 >= bytes.len() {

                            match identifier.as_str() {
                                "else" => tokens.push(Token::new(TokenKind::Preprocessor(PreprocessorKind::Else), line, line - line_offset, start_column, path.clone())),
                                "endif" => tokens.push(Token::new(TokenKind::Preprocessor(PreprocessorKind::EndIf), line, line - line_offset, start_column, path.clone())),
                                _ => {}
                            }

                            i+=1;
                            continue;
                        }

                        if bytes[i] != b' ' && bytes[i] != b'\n' {
                            let mut identifier = identifier.clone();
                            while i < bytes.len() && (bytes[i] != b'\n') {
                                identifier.push(bytes[i] as char);
                                i += 1;
                                column += 1;
                            }
                            i -= 1;
                            tokens.push(Token::new(TokenKind::Preprocessor(PreprocessorKind::Unknown(identifier)), line, line - line_offset, start_column + 1, path.clone()));

                            // i += 1;
                            continue;
                        }

                        let ide2 = identifier.clone();
                        match identifier.as_str()
                        {
                            "define" => {
                                identifier.clear();
                                let mut skipend = false;
                                while i < bytes.len() && (bytes[i] != b'\n' && !skipend) {
                                    if bytes[i] == b'\n' { line += 1; column = 0; }
                                    skipend = if bytes[i] == b'\\' { true } else { false };
                                    identifier.push(bytes[i] as char);
                                    i += 1;
                                    column += 1;
                                }
                                i -= 1;
                                tokens.push(Token::new(TokenKind::Preprocessor(PreprocessorKind::Define(identifier.trim().to_string())), line, line - line_offset, start_column + 1, path.clone()));
                            },
                            _ => {
                                let mut after = String::new();
                                while i < bytes.len() && (bytes[i] != b'\n') {
                                    after.push(bytes[i] as char);
                                    i += 1;
                                    column += 1;
                                }
                                // i -= 1;
                                after = after.trim().to_string();
                                match ide2.as_str()
                                {
                                    "if" => tokens.push(Token::new(TokenKind::Preprocessor(PreprocessorKind::If(after)), line, line - line_offset, start_column, path.clone())),
                                    "ifdef" => tokens.push(Token::new(TokenKind::Preprocessor(PreprocessorKind::Ifdef(after)), line, line - line_offset, start_column, path.clone())),
                                    "ifndef" => tokens.push(Token::new(TokenKind::Preprocessor(PreprocessorKind::Ifndef(after)), line, line - line_offset, start_column, path.clone())),
                                    "elif" => tokens.push(Token::new(TokenKind::Preprocessor(PreprocessorKind::Elif(after)), line, line - line_offset, start_column, path.clone())),
                                    "undef" => tokens.push(Token::new(TokenKind::Preprocessor(PreprocessorKind::Undefine(after)), line, line - line_offset, start_column, path.clone())),
                                    "else" => tokens.push(Token::new(TokenKind::Preprocessor(PreprocessorKind::Else), line, line - line_offset, start_column, path.clone())),
                                    "endif" => tokens.push(Token::new(TokenKind::Preprocessor(PreprocessorKind::EndIf), line, line - line_offset, start_column, path.clone())),
                                    "line" => {

                                        if after.is_empty() {
                                            return Err(format!("Invalid line directive at line {}", line));
                                        } else {

                                            let split = after.split_whitespace().collect::<Vec<&str>>();
                                            if split.is_empty() { return Err(format!("Invalid line directive at line {}", line)); }
                                            
                                            let newlinestr = split[0];

                                            let newline = newlinestr.parse::<u32>();
                                            if !newline.is_ok() { return Err(format!("Invalid line directive at line '{}', expected positive integer", newlinestr)); }
                                            
                                            let file = if split.len() > 1
                                            {
                                                Some(split.iter().skip(1).map(|s| s.to_string()).collect::<Vec<String>>().join(" ").clone().trim_matches('"').to_string())
                                            } else { None };

                                            if file.is_some() {
                                                path = file.clone().unwrap();
                                            }

                                            // line = newline.unwrap() as usize;
                                            let start_line = newline.unwrap();
                                            line_offset = i64::max(line as i64 - start_line as i64, 0) as usize;
                                            if line_offset > line { return Err(format!("Invalid line directive at line {}, cannot be greater than current line", newlinestr)); }
                                            column = 0;
                                            tokens.push(Token::new(TokenKind::Preprocessor(PreprocessorKind::Line((start_line, file))), line, line - line_offset, start_column, path.clone()));
                                        }
                                    },
                                    _ => tokens.push(Token::new(TokenKind::Preprocessor(PreprocessorKind::Unknown(ide2 + " " + &after)), line, line - line_offset, start_column, path.clone())),
                                }
                            }
                        }
                    }

                    continue;
                }

                while i < bytes.len() && (bytes[i].is_ascii_alphabetic() || bytes[i] == b'_' || bytes[i].is_ascii_digit()) {
                    identifier.push(bytes[i] as char);
                    i += 1;
                    column += 1;
                }

                if !identifier.is_empty()
                {
                    i -= 1;
                    // column -= 1; // should remain

                    match identifier.as_str() {
                        "export" | "struct" | "in" | "out" | "inout" => tokens.push(Token::new(TokenKind::Keyword(identifier), line, line - line_offset, start_column, path.clone())),
                        _ => tokens.push(Token::new(TokenKind::Identifier(identifier), line, line - line_offset, start_column, path.clone()))
                    }
                } else {
                    tokens.push(Token::new(TokenKind::Symbol(c as char), line, line - line_offset, column, path.clone()));
                }
            },
        }
        i += 1;
    }

    Ok(tokens)
}

fn format_error(line: &str, token: &Token, message: &str) -> String {
    let mut error = String::new();
    error.push_str(&format!("In: {}:{}\n", token.path, token.line));
    error.push_str(&format!("Error: {} at line {} column {} | {}\n", message, token.line, token.column, token.global_line));
    let line_mark = format!("{}: ", token.line);
    error.push_str(&format!("{}\n", line_mark.clone() + line));
    error.push_str(&format!("{}\n", " ".repeat(line_mark.len() + usize::max(1, token.column) - 1) + "^".repeat(token.kind.length()).as_str()));
    error
}

fn format_multi_error(prev_line: &str, line: &str, prev: &Token, token: &Token, message: &str) -> String {
    let mut error = String::new();
    error.push_str(&format!("Error: {} at line {} column {} | {}\n", message, token.line, token.column, token.global_line));

    let line_mark = format!("{}: ", prev.line);
    error.push_str(&format!("{}\n", line_mark.clone() + prev_line));
    error.push_str(&format!("{}\n", " ".repeat(line_mark.len() + usize::max(1, prev.column) - 1) + "^".repeat(prev.kind.length()).as_str()));

    let line_mark = format!("{}: ", token.line);
    error.push_str(&format!("{}\n", line_mark.clone() + line));
    error.push_str(&format!("{}\n", " ".repeat(line_mark.len() + usize::max(1, token.column) - 1) + "^".repeat(token.kind.length()).as_str()));

    error
}

fn is_resource_type(str: &str) -> bool {
    match str {
        "cbuffer" | "SamplerState" | "Sampler" |
        "ConstantBuffer" | "Texture2D" | "TextureCube" | "Texture2DArray" | "Texture3D" => true,
        _ => false,
    }
}

// const BASE_SCALAR_TYPES: &'static [&'static str] = &["int", "uint", "dword", "half", "float", "double"];
// const EXTENDED_SCALAR_TYPES: &'static [&'static str] = &["f16", "f32", "f64"];

macro_rules! join_with_pipe {
    ($($item:expr),*) => {
        $(concat!("\"", $item, "\"")) | *
    };
}

macro_rules! join_with_pipe_vec {
    ($($item:expr),*) => {
        $(concat!("\"", $item, "1\"") |
          concat!("\"", $item, "2\"") |
          concat!("\"", $item, "3\"") |
          concat!("\"", $item, "4\"")) | *
    };
}

fn is_scalar_type(str: &str) -> bool {
    match str {
        join_with_pipe!("int", "uint", "dword", "half", "float", "double") => true,
        join_with_pipe!("f16", "f32", "f64") => true,
        _ => false,
    }
}

fn is_vector_type(str: &str) -> bool {
    match str {
        join_with_pipe_vec!("int", "uint", "dword", "half", "float", "double") => true,
        _ => false,
    }
}

fn is_texture_compatible_type(str: &str) -> bool {
    match str {
        "float" => true,
        _ => false,
    }
}

fn is_resource_texture(str: &str) -> bool {
    match str {
        "Texture2D" | "TextureCube" | "Texture2DArray" | "Texture3D" | "TextureCubeArray" | "TextureBuffer" | "TextureBufferArray" => true,
        _ => false,
    }
}

fn is_resource_overloadable(str: &str) -> bool {
    match str {
        "ConstantBuffer" => true,
        _ if is_resource_texture(str) => true,
        _ => false,
    }
}

fn is_type(str: &str, custom_types: &HashSet<String>) -> bool {
    match str {
        _ if is_resource_type(str) => true,
        "void" | "int" | "float" | "bool" | "vec2" | "vec3" | "vec4" | "mat4" => true,
        _ => custom_types.contains(&str.to_string()),
    }
}

#[derive(Debug)]
struct Binding {
    name: String,
    type_name: String,
    binding: u32,
    body: Option<String>,
}

impl Binding {
    fn new(name: String, type_name: String, binding: u32, body: Option<String>) -> Self {
        Self { name, type_name, binding, body }
    }
}

#[derive(Debug, Clone, PartialEq)]
enum BindingDomain {
    Texture,
    Sampler,
    Buffer,
}

#[derive(Debug, Clone, PartialEq)]
enum BindingType {
    ConstantBuffer,
    StructuredBuffer,
    CBuffer,
    Sampler,

    Texture1D,
    Texture1DArray,
    Texture2D,
    TextureCube,
    Texture2DArray,
    Texture3D,
    TextureCubeArray,
    TextureBuffer,
    TextureBufferArray,
}

impl BindingType {

    fn from_str(str: &str) -> Option<Self> {
        match str {
            "ConstantBuffer"=> Some(Self::ConstantBuffer),
            "cbuffer" => Some(Self::CBuffer),
            "Texture2D" => Some(Self::Texture2D),
            "Sampler" | "SamplerState" => Some(Self::Sampler),
            "Texture1D" => Some(Self::Texture1D),
            "Texture1DArray" => Some(Self::Texture1DArray),
            "Texture2DArray" => Some(Self::Texture2DArray),
            "Texture3D" => Some(Self::Texture3D),
            "TextureCube" => Some(Self::TextureCube),
            "TextureCubeArray" => Some(Self::TextureCubeArray),
            "TextureBuffer" => Some(Self::TextureBuffer),
            "TextureBufferArray" => Some(Self::TextureBufferArray),
            "StructuredBuffer" => Some(Self::StructuredBuffer),
            _ => None,
        }
    }

    fn to_str(&self) -> &str {
        match self {
            Self::ConstantBuffer => "ConstantBuffer",
            Self::CBuffer => "cbuffer",
            Self::Texture2D => "Texture2D",
            Self::Sampler => "SamplerState",
            Self::Texture1D => "Texture1D",
            Self::Texture1DArray => "Texture1DArray",
            Self::Texture2DArray => "Texture2DArray",
            Self::Texture3D => "Texture3D",
            Self::TextureCube => "TextureCube",
            Self::TextureCubeArray => "TextureCubeArray",
            Self::TextureBuffer => "TextureBuffer",
            Self::TextureBufferArray => "TextureBufferArray",
            Self::StructuredBuffer => "StructuredBuffer",
        }
    }

    fn is_overloadable(&self) -> bool {
        match self {
            Self::ConstantBuffer => true,
            Self::Texture2D => true,
            Self::Texture1D => true,
            Self::Texture1DArray => true,
            Self::Texture2DArray => true,
            Self::Texture3D => true,
            Self::TextureCube => true,
            Self::TextureCubeArray => true,
            Self::TextureBuffer => true,
            Self::TextureBufferArray => true,
            Self::StructuredBuffer => true,
            _ => false,
        }
    }

    fn has_body(&self) -> bool {
        return self == &BindingType::CBuffer;
    }

    fn get_domain(&self) -> BindingDomain {
        match self {
            Self::Texture2D |
            Self::Texture1D |
            Self::Texture1DArray |
            Self::Texture2DArray |
            Self::Texture3D |
            Self::TextureCube |
            Self::TextureCubeArray |
            Self::TextureBuffer |
            Self::TextureBufferArray => BindingDomain::Texture,
            
            Self::ConstantBuffer |
            Self::StructuredBuffer |
            Self::CBuffer => BindingDomain::Buffer,

            Self::Sampler => BindingDomain::Sampler,
        }
    }
}

#[derive(Debug)]
struct BindingInfo {
    binding_type: BindingType,
    name: String,
    overload: Option<String>,
    binding_count: Option<u32>,
    body: Option<String>,
}

impl BindingInfo {
    fn new(binding_type: BindingType, name: String) -> Self {
        Self {
            binding_type,
            name,
            overload: None,
            binding_count: None,
            body: None,
        }
    }
}

fn parse_binding(binding_type: &BindingType, current: &Token, iter: &mut TokenIter, declared_structs: &HashSet<String>, lines:  &[&str]) -> Result<BindingInfo, String> {
    // assume binding_type was validated

    let mut _overload = None;
    
    if iter.expect_symbol(Some('<'), None).is_ok()
    {
        if !binding_type.is_overloadable() {
            return Err(format_error(lines[current.global_line - 1], current, "Binding type is not overloadable"));
        }

        let buff = *binding_type == BindingType::StructuredBuffer || *binding_type == BindingType::ConstantBuffer;

        let overload_token = iter.expect_identifier(None, if buff { Some("expected struct name") } else { Some("expected type name") })?;
        let overload = overload_token.to_string();

        if buff {
            if !declared_structs.contains(&overload) {
                return Err(format_error(lines[current.global_line - 1], current, &format!("struct '{}' not declared", overload)));
            }
        } else {
            if !is_scalar_type(&overload) && !is_vector_type(&overload) {
                return Err(format_error(lines[current.global_line - 1], current, &format!("type '{}' is not a valid type", overload)));
            }
        }

        _overload = Some(overload);

        iter.expect_symbol(Some('>'), None)?;
    }

    let name_token = iter.expect_identifier(None, Some("name"))?;
    let name = name_token.to_string();
    
    let mut info: BindingInfo = BindingInfo::new(binding_type.clone(), name.to_string());
    info.overload = _overload;

    if iter.expect_symbol(Some('['), None).is_ok()
    {
        if *binding_type == BindingType::CBuffer || *binding_type == BindingType::Sampler {
            // Intentionally error at type instead of [
            return Err(format_error(lines[current.global_line - 1], current, &format!("binding type '{}' cannot be an array", binding_type.to_str())));
        } else {
            let count_token = iter.expect_integer(None, Some("expected binding count"))?;

            let count = count_token
                .to_string()
                .parse::<u32>()
                .map_err(|_| {
                    format_error(
                        lines[count_token.global_line - 1],
                        count_token,
                        "invalid binding array size"
                    )
                })?;

            info.binding_count = Some(count);

            iter.expect_symbol(Some(']'), None)?;
        }
    }


    if *binding_type == BindingType::CBuffer {
        let body_token = iter.expect_body(None)?;
        iter.expect_terminator(Some(';'), None)?;

        info.body = Some(body_token.to_string());
    } else {
        iter.expect_terminator(Some(';'), None)?;
    }
    
    Ok(info)
}

#[derive(Debug)]
struct ParseResult {
    src: String,
    module_type: Option<String>,
    bindings: Vec<Vec<Binding>>,
    main_functions: Vec<String>,
    // structs: HashSet<String>,
    // functions: HashSet<String>,
    // defined_functions: HashSet<String>,
}

fn parse_stage1(tokens: Vec<Token>, original_src: &str) -> Result<ParseResult, String> {
    let mut parse_result = ParseResult {
        src: String::new(),
        module_type: None,
        bindings: vec![vec![]],
        main_functions: vec![],
    };

    let mut current_binding_space = "".to_string();

    let mut iter = TokenIter::new(tokens.iter(), original_src);

    let mut custom_types = HashSet::<String>::new();
    let mut structs = HashSet::<String>::new();
    let mut functions = HashSet::<String>::new();
    let mut defined_structs = HashSet::<String>::new();
    let mut defined_functions = HashSet::<String>::new();

    let mut verify_defines = true;
    let mut verify_struct_declarations_token: HashMap<String, Token> = HashMap::new();
    let mut verify_function_declarations_token: HashMap<String, Token> = HashMap::new();

    let mut type_token: Option<Token> = None;

    let src_split = original_src.split('\n').collect::<Vec<&str>>();

    let mut impls_token: HashMap<String, Token> = HashMap::new();
    let mut imports_token: HashMap<String, Token> = HashMap::new();
    let mut last_if = None;

    let mut macro_depth = 0;

    while let Some(token) = iter.next() {
        match &token.kind {
            TokenKind::Keyword(keyword) => {
                match keyword.as_str() {
                    "struct" => {

                        let name_token = iter.expect_identifier(None, Some("name"))?;
                        let name = name_token.to_string();

                        let first_char = name.chars().next().unwrap();

                        if !first_char.is_ascii_alphabetic()
                        {
                            let err_msg = format!("Expected struct name to start with an alphabet character but got '{}'", first_char);
                            return Err(format_error(src_split[name_token.global_line - 1],  name_token, err_msg.as_str()));
                        }

                        parse_result.src.push_str(&format!("struct {}", name));
                        let body_token = iter.expect_body(None).and_then(|body| {
                            if defined_structs.contains(&name) {
                                return Err(format_error(src_split[name_token.global_line - 1],  name_token, "Struct already defined"));
                            }
                            if verify_defines {
                                verify_struct_declarations_token.remove(&name);
                            }
                            parse_result.src.push_str(format!("\n{}", body.to_string()).as_str());
                            Ok(())
                        }).or_else(|err| {
                            if verify_defines {
                                let _ = verify_struct_declarations_token.insert(name.clone(), name_token.clone());
                            }
                            Err(err)
                        });

                        iter.expect_terminator(Some(';'), Some("';' or '{'"))?;
                        parse_result.src.push_str(";\n");

                        custom_types.insert(name.clone());
                        defined_structs.insert(name);

                        
                        
                        // let body_or_terminator = iter.expect_next_token(Some("body or ';'"))?;

                        // match &body_or_terminator.kind {
                        //     TokenKind::CodeBlock(body) => {
                        //         parse_result.src.push_str(&format!("struct {} \n{}\n", name, body));
                        //     },
                        //     TokenKind::Terminator(';') => {
                        //         parse_result.src.push_str(&format!("struct {};\n", name));
                        //     },
                        //     _ => {

                        //         let err_msg = format!("Expected struct body or ';' but got {}", body_or_terminator.kind.to_string());
                        //         return Err(format_error(src_split[body_or_terminator.global_line - 1], body_or_terminator, &err_msg));
                        //     },
                        // }
                    },
                    _ => {},
                }
            },
            TokenKind::Identifier(identifier) => {
                if identifier.chars().next().unwrap().is_ascii_alphabetic() && is_type(identifier, &custom_types) {

                    // IS RESOURCE
                    if let Some(binding_type) = BindingType::from_str(identifier) {
                        println!("Resource type: {}", identifier);


                        let binding_info = parse_binding(&binding_type, &token, &mut iter, &defined_structs, &src_split)?;


                        parse_result.src.push_str(format!("\n#line {} {}", token.line, token.path).as_str());
                        parse_result.src.push_str(format!("\n@binding({},{},{},\"{}\")", 
                            binding_info.binding_type.to_str(),
                            binding_info.name,
                            binding_info.binding_count.unwrap_or(1),
                            current_binding_space
                        ).as_str());

                        parse_result.src.push('\n');

                        parse_result.src.push_str(&format!("{}", binding_type.to_str()));
                        
                        if let Some(overload) = binding_info.overload {
                            parse_result.src.push_str(&format!("<{}>", overload));
                        }

                        parse_result.src.push(' ');

                        parse_result.src.push_str(&binding_info.name);

                        if let Some(count) = binding_info.binding_count {
                            parse_result.src.push_str(&format!("[{}]", count));
                        }

                        if binding_type.has_body() && let Some(body) = binding_info.body {
                            parse_result.src.push_str(&format!("\n{}", body));
                        }
                        parse_result.src.push(';');
                        parse_result.src.push('\n');


                    } else {

                        let name_token = iter.expect_identifier(None, None)?;
                        
                        // IS FUNCTION
                        if iter.expect_symbol(Some('('), None).is_ok() {

                            let mut name = name_token.to_string();

                            // match name_token.to_string().as_str() {
                            //     "MainVS" | "MainPS" | "MainCS" | "MainGS" => {},
                            //     _ => {
                            //         let path_hash = xxhash_rust::xxh3::xxh3_64_with_seed(token.path.as_bytes(), 0);
                                    
                            //         name.push_str(&format!("_{:016x}", path_hash));
                            //     }
                            // }

                            parse_result.src.push_str(&format!("{} {}", identifier, name));

                            parse_result.src.push('(');
                            if iter.expect_symbol(Some(')'), None).is_ok()
                            {
                                iter.expect_terminator(Some(';'), None)?;
                                parse_result.src.push_str(")\n");
                            }

                            let exit = false;
                            while !exit {

                                {
                                    let kw = iter.expect_keyword(None, None);
                                    if kw.is_ok()
                                    {
                                        match kw.clone().unwrap().to_string().as_str()
                                        {
                                            "in" => { parse_result.src.push_str("in "); },
                                            "out" => { parse_result.src.push_str("out "); },
                                            "inout" => { parse_result.src.push_str("inout "); },
                                            _ => {
                                                return Err(format_error(src_split[kw.clone().unwrap().global_line - 1], &kw.clone().unwrap(), "'in', 'out' or 'inout'"));
                                            },
                                        }
                                    }
                                }

                                let type_token = iter.expect_identifier(None, Some("param type"))?;
                                let name_token = iter.expect_identifier(None, Some("param name"))?;

                                if is_type(&type_token.to_string(), &custom_types) {                                
                                    parse_result.src.push_str(&format!("{} {}", type_token.to_string(), name_token.to_string()));

                                    if iter.expect_symbol(Some(','), None).is_err() {
                                        iter.expect_symbol(Some(')'), None)?;
                                        parse_result.src.push(')');
                                        break;
                                    }

                                    parse_result.src.push_str(", ");
                                } else {
                                    return Err(format_error(src_split[token.global_line - 1], token, &format!("Expected param type but got '{}'", identifier)));
                                }
                            }

                            if iter.expect_terminator(Some(';'), None).is_ok()
                            {
                                parse_result.src.push(';');
                                parse_result.src.push('\n');
                                
                                if verify_defines {
                                    verify_function_declarations_token.insert(name_token.to_string(), name_token.clone());
                                }
                            } else {
                                if defined_functions.contains(&name_token.to_string()) {
                                    return Err(format_error(src_split[token.global_line - 1], token, &format!("Function '{}' is already defined", name_token.to_string())));
                                }

                                if verify_defines {
                                    verify_function_declarations_token.remove(&name_token.to_string());
                                }

                                defined_functions.insert(name_token.to_string());
                                let body_token = iter.expect_body(None)?;
                                parse_result.src.push_str(&format!("\n{}\n", body_token.to_string()));
                            }

                            functions.insert(name_token.to_string());

                            
                            match name_token.to_string().as_str() {
                                "MainVS" => parse_result.main_functions.push(name_token.to_string()),
                                "MainPS" => parse_result.main_functions.push(name_token.to_string()),
                                "MainCS" => parse_result.main_functions.push(name_token.to_string()),
                                "MainGS" => parse_result.main_functions.push(name_token.to_string()),
                                _ => {}
                            }
                        } else {
                            return Err(format_error(src_split[token.global_line - 1], token, &format!("Unknown syntax '{}'", identifier).as_str()));
                        }
                    }

                } else {
                    return Err(format_error(src_split[token.global_line - 1], token, &format!("Unknown type '{}'", identifier).as_str()));
                }
            },
            TokenKind::Import => {
                let import = iter.expect_literal(None, None)?;
                iter.expect_terminator(Some('\n'), None)?;

                if imports_token.contains_key(&import.to_string()) {
                    return Err(format_error(src_split[token.global_line - 1], token, &format!("Error: Module already imported at line {} | Error", import.line).as_str()));
                }
                
                imports_token.insert(import.to_string(), import.clone());
                
                parse_result.src.push_str(&format!("\n@import \"{}\"\n", import.to_string()));
            },
            TokenKind::Type => {
                if parse_result.module_type.is_some() {
                    return Err(
                        format_error(src_split[token.global_line - 1], token,
                        &format!("Error: Module type already defined at line {} | Error", type_token.unwrap().line).as_str())
                    );
                }
                let type_token_extr = iter.expect_literal(None, None)?;
                type_token = Some(type_token_extr.clone());
                parse_result.module_type = Some(type_token_extr.to_string());
            },
            TokenKind::Space => {
                let space = iter.expect_literal(None, None)?;
                if iter.peek().is_some() {
                    iter.expect_terminator(Some('\n'), None)?;
                }
                current_binding_space = space.to_string();
            },
            TokenKind::Implements => {
                let file = iter.expect_literal(None, None)?;
                
                parse_result.src.push_str(&format!("\n@implements \"{}\"\n", file.to_string()));

                if impls_token.contains_key(&file.to_string()) {
                    return Err(format_error(src_split[token.global_line - 1], token, &format!("Error: Module already implemented at line {} | Error", file.line).as_str()));
                }

                impls_token.insert(file.to_string(), file.clone());
            
                
                if iter.peek().is_some() {
                    iter.expect_terminator(Some('\n'), None)?;
                }
            },
            TokenKind::NoRequiredDefineStart => {
                if iter.peek().is_some() {
                    iter.expect_terminator(Some('\n'), None)?;
                }

                verify_defines = false;
            },
            TokenKind::NoRequiredDefineEnd => {
                if iter.peek().is_some() {
                    iter.expect_terminator(Some('\n'), None)?;
                }

                verify_defines = true;
            },
            TokenKind::Preprocessor(preprocessor) => {
                match preprocessor {
                    PreprocessorKind::Line(line) => {
                        if line.1.is_some() {
                            parse_result.src.push_str(&format!("\n#line {} {}\n", line.0, line.1.clone().unwrap()));
                        } else {
                            parse_result.src.push_str(&format!("\n#line {}\n", line.0));
                        }
                    },
                    PreprocessorKind::If(str) => {
                        parse_result.src.push_str(&format!("\n#if {}\n", str));
                        last_if = Some(token.clone());
                        macro_depth += 1;
                        println!("#IF {}", str);
                    },
                    PreprocessorKind::Ifdef(str) => {
                        parse_result.src.push_str(&format!("\n#ifdef {}\n", str));
                        last_if = Some(token.clone());
                        macro_depth += 1;
                        println!("#IFDEF {}", str);
                    },
                    PreprocessorKind::Ifndef(str) => {
                        parse_result.src.push_str(&format!("\n#ifndef {}\n", str));
                        last_if = Some(token.clone());
                        macro_depth += 1;
                        println!("#IFNDEF {}", str);
                    },
                    PreprocessorKind::Elif(str) => {
                        if macro_depth > 0 {
                            parse_result.src.push_str(&format!("\n#elif {}\n", str));
                            last_if = Some(token.clone());
                            println!("#ELIF {}", str);
                        } else {
                            return Err(format_error(src_split[token.global_line - 1], token, "Error: Elif without If"));
                        }
                    },
                    PreprocessorKind::Else => {
                        if macro_depth > 0 {
                            parse_result.src.push_str("\n#else\n");
                            last_if = Some(token.clone());
                            println!("#ELSE");
                        } else {
                            return Err(format_error(src_split[token.global_line - 1], token, "Error: Else without If"));
                        }
                    },
                    PreprocessorKind::EndIf => {
                        if macro_depth > 0 {
                            parse_result.src.push_str("\n#endif\n");
                            macro_depth -= 1;
                            println!("#ENDIF");
                        } else {
                            return Err(format_error(src_split[token.global_line - 1], token, "Error: EndIf without If"));
                        }
                    },
                    _ => {},
                }
            },
            TokenKind::Terminator(terminator) => parse_result.src.push(*terminator),
            _ => {
                return Err(format_error(src_split[token.global_line - 1], token, "Error: Invalid token"));
            },
        }
    }

    if macro_depth > 0 {
        return Err(format_error(src_split.last().unwrap(), &last_if.unwrap(), &format!("Error: EndIf missing, {}", macro_depth)));
    }

    if parse_result.main_functions.len() > 0 {
        if impls_token.len() > 0 {   
            let token = impls_token.iter().next().unwrap().1;
            return Err(format_error(src_split[token.global_line - 1], token, "Error: An entry point module cannot implement interfaces meant for dynamic modules"));
        }
    } else {
        if imports_token.len() > 0 {
            let token = imports_token.iter().next().unwrap().1;
            return Err(format_error(src_split[token.global_line - 1], token, "Error: A non-entry point module cannot import dynamic modules"));
        }
    }

    for (name, token) in verify_struct_declarations_token {
        return Err(format_error(src_split[token.global_line - 1], &token, &format!("Struct {} not defined but declared", name).as_str()));
    }

    for (name, token) in verify_function_declarations_token {
        return Err(format_error(src_split[token.global_line - 1], &token, &format!("Function {} not defined but declared", name).as_str()));
    }

    Ok(parse_result)
}

fn preprocess_old(original_path: &std::path::PathBuf, include_paths: &Vec<&str>) -> Result<String, String> {
    let mut out = String::new();

    let src = std::fs::read_to_string(original_path);

    if src.is_err() {
        return Err("Unable to read file".to_string());
    }

    struct Define {
        name: String,
        params: Vec<String>,
        body: String,
    }

    let src = src.unwrap();

    let mut defines = HashMap::new();
    let mut included = HashSet::new();

    let lines = src.lines().collect::<Vec<&str>>();
    
    let mut line_num = 1;
    let mut iter = lines.iter();
    while let Some(line) = iter.next() {
        let trimmed = line.trim();
        
        if trimmed.starts_with("#") && trimmed.len() > 1 {
            let trimmed2 = trimmed[1..].trim();

            let directive = trimmed2.split_whitespace().into_iter().next().unwrap();

            match directive {
                "include" => {
                    let path = trimmed2["include".len()..].trim();
                    let mut path = path[1..path.len() - 1].trim().to_string();

                    let mut found = false;
                    for include_path in include_paths {
                        let full_path = include_path.to_string() + "/" + path.as_str();
                        
                        if std::path::Path::new(&full_path).exists() {
                            found = true;
                            path = full_path;
                            break;
                        }
                    }

                    if !found {
                        return Err(format!("Include path {} not found", path));
                    }

                    if path == original_path.to_str().unwrap() {
                        let fake_token = Token::new(TokenKind::Terminator('\n'), 0, line_num, 0, original_path.to_str().unwrap().to_string());
                        return Err(format_error(&lines[line_num - 1], &fake_token, "Include path cannot be the same as the original path"));
                    }

                    if included.contains(&path) {
                        continue;
                    }

                    included.insert(path.clone());

                    let include_src = preprocess_old(&std::path::PathBuf::from(path.clone()), include_paths);
                    if include_src.is_err() { return Err(include_src.unwrap_err()); }
                    let include_src = include_src.unwrap();
                    out.push_str(&format!("\n#line 1 \"{}\"\n", path));
                    out.push_str(&include_src);
                    out.push_str(&format!("\n#line {} \"{}\"\n", line_num, path));
                    line_num += 1;
                    continue;
                },
                "define" => {
                    let trimmed2 = trimmed2["define".len()..].trim();
                    let tokenize = tokenize(trimmed2)?;

                    let mut token_iter = TokenIter::new(tokenize.iter(), src.as_str());

                    let name_token = token_iter.expect_identifier(None, Some("Expected define name!"))?;

                    let name = name_token.clone().to_string();

                    // if defines.contains_key(&name) {
                    //     return Err(format_error(&line, &name_token, &format!("Define {} already defined", name)));
                    // }

                    defines.remove(&name);
                    
                    let define = &defines.insert(name.clone(), Define {
                        name: name.clone(),
                        params: Vec::new(),
                        body: String::new(),
                    });

                    if token_iter.expect_symbol(Some('('), None).is_ok() {
                        let mut bad_exit = None;
                        while let Some(token) = token_iter.next() {
                            match &token.kind {
                                TokenKind::Identifier(param_name) => {
                                    bad_exit = Some(token.clone());
                                    if token_iter.expect_symbol(Some(','), None).is_ok() {
                                        defines.get_mut(&name.to_string()).unwrap().params.push(param_name.clone().to_string());
                                    } else {
                                        token_iter.expect_symbol(Some(')'), None)?;
                                        defines.get_mut(&name.to_string()).unwrap().params.push(param_name.clone().to_string());
                                        bad_exit = None;
                                        break;
                                    }
                                }
                                _ => {
                                    return Err(format_error(&line, &token, &format!("Expected identifier but got {}", token.kind.to_string())));
                                }
                            }
                        }

                        if let Some(token) = bad_exit {
                            return Err(format_error(&line, &token, &format!("Expected ) but got {}", token.kind.to_string())));
                        }
                    }

                    let trimmed_end = line.trim_end();

                    let mut multi_lines = trimmed_end.ends_with('\\');

                    let mut value = String::new();

                    let name_end = if defines.get(&name).unwrap().params.is_empty() {
                        line.find(&name).unwrap() + name.len() + 1
                    } else {
                        line.find(')').unwrap() + 2
                    };

                    let str = if multi_lines {
                        &line[name_end..line.trim_end().len()-1]
                    } else {
                        if line.len() < name_end {
                            ""
                        } else {
                            &line[name_end..line.trim_end().len()]
                        }
                    };

                    if str.is_empty() {
                        line_num += 1;
                        continue;   
                    }

                    value.push_str(&str);

                    println!("MULTI_LINE: {}", multi_lines);
                    if multi_lines {
                        while let Some(line) = iter.next() {
                            line_num += 1;
                            let trimmed_line = line.trim_end();
                            if trimmed_line.ends_with('\\') {
                                multi_lines = true;
                                value.push('\n');
                                value.push_str(&trimmed_line[..trimmed_line.len()-1]);
                            } else {
                                multi_lines = false;

                                value.push('\n');
                                value.push_str(&trimmed_line);
                            }
                            if !multi_lines { break; }
                        }
                    }

                    defines.get_mut(&name.to_string()).unwrap().body = value.clone();

                    println!("Name: {:?}", defines.values().last().unwrap().name);
                    println!("Params: {:?}", defines.values().last().unwrap().params);
                    println!("Body: {:?}", defines.values().last().unwrap().body);

                    println!("{}", value);

                    // panic!();

                    line_num += 1;
                    continue;
                },
                "pragma" => {
                    let trimmed2 = trimmed2["pragma".len()..].trim();
                    if trimmed2.starts_with("once") {
                        line_num += 1;
                        continue;
                    }
                }
                _ => {}
            }
            
        } else if trimmed.starts_with("@") {
            let trimmed2 = trimmed[1..].trim();

            if trimmed2.starts_with("space") || trimmed2.starts_with("no_required_define_start") || trimmed2.starts_with("no_required_define_end") {
                return Err(format!("Illegal space definition"));                
            } else if trimmed2.starts_with("import") {
                let tokens = tokenize(&line)?;
                
                let mut iter = TokenIter::new(tokens.iter(), src.as_str());

                let imp = iter.expect_import_marker(None)?;
                
                if iter.peek().is_none() {
                    return Err(format_error(&line, imp, "Expected path or static built-in module"));
                }
                
                let path = iter.expect_literal(None, None);

                if path.is_err() {
                    return Err(format_error(&line, iter.peek().unwrap(), "Mapped imports are not supported yet!"));
                } else {
                    let path_token = path.unwrap();
                    let path = path_token.to_string();

                    if path.ends_with(".hsi") {
                        // interface file
                        
                        let mut path = path.to_string();

                        let mut found = false;
                        for include_path in include_paths {
                            let full_path = include_path.to_string() + "/" + path.as_str();
                            
                            if std::path::Path::new(&full_path).exists() {
                                found = true;
                                path = full_path;
                                break;
                            }
                        }

                        if !found {
                            return Err(format_error(&line, path_token, "Interface file not found"));
                        }

                        if path == original_path.to_str().unwrap() {
                            return Err(format_error(&line, path_token, "Interface file cannot be the same as the original file"));
                        }

                        if included.contains(&path) {
                            continue;
                        }

                        included.insert(path.clone());

                        out.push_str(&format!("\n@import \"{}\"\n", path));
                        out.push_str(&format!("\n@no_required_define_start\n"));                         
                        out.push_str(&format!("\n#line 1 \"{}\"\n", path));
    
                        let str = preprocess_old(&std::path::PathBuf::from(path.clone()), include_paths)?;
                        
                        out.push_str(&str);

                        out.push_str(&format!("\n@no_required_define_end\n"));                            
                        out.push_str(&format!("\n#line {} \"{}\"\n", line_num, path));
                        line_num += 1;
                        continue;

                    } else {

                        let mut path = path.to_string();

                        let mut found = false;
                        for include_path in include_paths {
                            let full_path = include_path.to_string() + "/" + path.as_str();
                            
                            if std::path::Path::new(&full_path).exists() {
                                found = true;
                                path = full_path;
                                break;
                            }
                        }

                        if !found {
                            return Err(format_error(&line, path_token, "Include file not found"));
                        }

                        if path == original_path.to_str().unwrap() {
                            return Err(format_error(&line, path_token, "Include path cannot be the same as the original path"));
                        }

                        if included.contains(&path) {
                            continue;
                        }

                        included.insert(path.clone());

                        out.push_str(&format!("\n@space \"{}\"\n", path));
                        out.push_str(&format!("\n#line 1 \"{}\"\n", path));
    
                        let str = preprocess_old(&std::path::PathBuf::from(path.clone()), include_paths)?;
                        
                        out.push_str(&str);
                    
                        out.push_str(&format!("\n#line {} \"{}\"\n", line_num, path));
                        
                        line_num += 1;
                        continue;
                    }
                }


            } else if trimmed2.starts_with("implements") {
                let tokens = tokenize(line)?;
                
                let mut iter = TokenIter::new(tokens.iter(), src.as_str());
                
                iter.expect_implements_marker(None)?;

                let path = iter.expect_literal(None, None);

                if path.is_err() {
                    return Err(format!("Mapped imports are not supported yet!"));
                } else {
                    let path_token = path.unwrap();
                    let mut path = path_token.to_string();

                    if !path.ends_with(".hsi") {
                        return Err(format_error(&line, path_token, "You may only implement HSL interfaces!"));
                    }

                    let mut found = false;
                    for include_path in include_paths {
                        let full_path = include_path.to_string() + "/" + path.as_str();
                        
                        if std::path::Path::new(&full_path).exists() {
                            found = true;
                            path = full_path;
                            break;
                        }
                    }

                    if !found {
                        return Err(format_error(&line, path_token, &format!("Interface path {} not found", path)));
                    }
                    
                    out.push_str(&format!("\n@implements \"{}\"\n", path));
                    out.push_str(&format!("\n#line 1 \"{}\"\n", path));

                    let str = preprocess_old(&std::path::PathBuf::from(path.clone()), include_paths)?;
                    
                    out.push_str(&str);
                    
                    out.push_str(&format!("\n#line {} \"{}\"\n", line_num, path));
                    line_num += 1;
                    continue;

                }
            }
        }

        let mut str = line.clone().to_string();

        for define in &defines {
            str = str.replace(&define.1.name, &define.1.body);
        }

        out.push_str(&str);
        out.push('\n');
        line_num += 1;
    }

    Ok(out)
}

fn stage1_tokens(path: &std::path::PathBuf, include_paths: &Vec<&str>) -> Result<Vec<Token>, String> {
    let src = preprocess_old(path, include_paths)?;

    let tokens = tokenize(src.as_str())?;

    Ok(tokens)
}

trait BindingAssigner {
    fn assign_slot(&mut self, binding_type: &BindingType) -> String;
    fn new(module_space: u32) -> Self;
}

struct VkBindingAssigner {
    texture_or_sampler_slot_counter: u32,
    buffer_slot_counter: u32,
    module_space: u32,
}

impl BindingAssigner for VkBindingAssigner {
    fn assign_slot(&mut self, binding_type: &BindingType) -> String {
        match binding_type.get_domain() {
            BindingDomain::Texture => {
                let slot = self.texture_or_sampler_slot_counter;
                self.texture_or_sampler_slot_counter += 1;
                return format!("t{}, space{}", slot, self.module_space);
            },
            BindingDomain::Sampler => {
                let slot = self.texture_or_sampler_slot_counter;
                self.texture_or_sampler_slot_counter += 1;
                return format!("s{}, space{}", slot, self.module_space);
            },
            BindingDomain::Buffer => {
                let slot = self.buffer_slot_counter;
                self.buffer_slot_counter += 1;
                return format!("b{}, space{}", slot, self.module_space);
            },
        }
    }

    fn new(module_space: u32) -> Self {
        Self {
            texture_or_sampler_slot_counter: 0,
            buffer_slot_counter: 0,
            module_space,
        }
    }
}

struct D3D11BindingAssigner {
    texture_slot_counter: u32,
    sampler_slot_counter: u32,
    buffer_slot_counter: u32
}

impl BindingAssigner for D3D11BindingAssigner {
    fn assign_slot(&mut self, binding_type: &BindingType) -> String {
        match binding_type.get_domain() {
            BindingDomain::Texture => {
                let slot = self.texture_slot_counter;
                self.texture_slot_counter += 1;
                return format!("t{}", slot);
            },
            BindingDomain::Sampler => {
                let slot = self.sampler_slot_counter;
                self.sampler_slot_counter += 1;
                return format!("s{}", slot);
            },
            BindingDomain::Buffer => {
                let slot = self.buffer_slot_counter;
                self.buffer_slot_counter += 1;
                return format!("b{}", slot);
            },
        }
    }

    fn new(_: u32) -> Self {
        Self {
            texture_slot_counter: 0,
            sampler_slot_counter: 0,
            buffer_slot_counter: 0
        }
    }
}


enum GraphicsAPI {
    Vulkan,
    D3D11,
}

enum AnyBindingAssigner {
    Vulkan(VkBindingAssigner),
    D3D11(D3D11BindingAssigner),
}

impl BindingAssigner for AnyBindingAssigner {
    fn assign_slot(&mut self, binding_type: &BindingType) -> String {
        match self {
            AnyBindingAssigner::Vulkan(assigner) => assigner.assign_slot(binding_type),
            AnyBindingAssigner::D3D11(assigner) => assigner.assign_slot(binding_type),
        }
    }

    fn new(module_space: u32) -> Self {
        panic!("Use explicit constructor for AnyBindingAssigner");
    }
}

struct BindingManager {
    api: GraphicsAPI,
    binding_spaces: HashMap<String, AnyBindingAssigner>,
    module_space_counter: u32,
}

impl BindingManager {
    fn new(api: GraphicsAPI) -> Self {
        Self {
            api,
            binding_spaces: HashMap::new(),
            module_space_counter: 0,
        }
    }

    fn get_assigner(&mut self, module_space: &str) -> &mut AnyBindingAssigner {        
        if !self.binding_spaces.contains_key(module_space) {
            let module_space_id = self.module_space_counter;
            self.module_space_counter += 1;
            let assigner = match self.api {
                GraphicsAPI::Vulkan => AnyBindingAssigner::Vulkan(VkBindingAssigner::new(module_space_id)),
                GraphicsAPI::D3D11 => AnyBindingAssigner::D3D11(D3D11BindingAssigner::new(module_space_id)),
            };
            self.binding_spaces.insert(module_space.to_string(), assigner);
        }
        self.binding_spaces.get_mut(module_space).unwrap()
    }
}

fn parse_stage2_late(src: &str, api: GraphicsAPI) -> Result<String, String> {
    let mut out = String::new();

    println!("src: {}", src);
    
    let lines = src.lines().collect::<Vec<&str>>();
    let mut line_iter = lines.iter();

    let mut resources = HashSet::<String>::new();
    let mut binding_manager = BindingManager::new(api);

    let mut current_module_space = "".to_string();

    let mut current_last_set_name = "".to_string();
    let mut tmp_counter = 0;

    let mut line_num = 1;
    while let Some(line) = line_iter.next() {
        let line_trim = line.trim();
        let start_char = line_trim.chars().next();

        if start_char.is_none() {
            continue;
        }
        let start_char = start_char.unwrap();

        if start_char == '@' {
            let tokens = tokenize(line_trim)?;

            println!("Line: {}", line_trim);
            println!("Tokens: {:?}", tokens);

            let mut iter = TokenIter::new(tokens.iter(), line_trim);

            iter.expect_binding_marker(None)?;


            iter.expect_symbol(Some('('), None)?;
            let type_name_token = iter.expect_identifier(None, Some("Expected type name"))?;
            iter.expect_symbol(Some(','), None)?;
            let resource_name = iter.expect_identifier(None, Some("Expected resource name"))?;
            iter.expect_symbol(Some(','), None)?;
            let binding_count = iter.expect_integer(None, Some("Expected binding count"))?;
            iter.expect_symbol(Some(','), None)?;
            let binding_space_name = iter.expect_literal(None, Some("Expected binding space"))?;
            iter.expect_symbol(Some(')'), None)?;

            if iter.peek().is_some() {
                return Err(format!("Expected new line after binding"));
            }

            let type_name = type_name_token.to_string();
            let resource_name = resource_name.to_string();
            let binding_count = binding_count.to_string();
            let binding_space_name = binding_space_name.to_string();

            if resources.contains(&resource_name) {
                return Err(format!("Resource {} already defined", resource_name));
            }

            resources.insert(resource_name.clone());

            if let Some(line) = line_iter.next() {
                let line = line.replace('{', "");
                let tokens = tokenize(&line)?;
                
                let rev_tokens = tokens.iter().rev().collect::<Vec<&Token>>();
                
                let token = rev_tokens.iter().next();

                let binding_type = BindingType::from_str(&type_name);
                if binding_type.is_none() {
                    return Err(format!("Invalid binding type: {}", type_name));
                }
                let binding_type = binding_type.unwrap();

                if let Some(token) = token {
                    match &token.kind {
                        TokenKind::Terminator(';') => {
                            out.push_str(
                                &format!("{} : register({});\n", &line[..line.len() - 1], 
                                binding_manager.get_assigner(&binding_space_name).assign_slot(&binding_type))
                            );
                            tmp_counter += 1;
                        },
                        TokenKind::Identifier(str) => {
                            out.push_str(&format!("{} {} : register({}){}\n", line.replace(&*str, "").trim(), str, 
                                binding_manager.get_assigner(&binding_space_name).assign_slot(&binding_type), 
                                if type_name == "cbuffer" { "" } else { ";" }
                            ));
                            tmp_counter += 1;
                        },
                        _ => {
                            return Err(format!("Expected new line after binding"));
                        }   
                    }
                }
            } else {
                return Err(format!("Expected binding but got end of file"));
            }

            // dd
        } else if start_char == '#' {
            let tokens = tokenize(line_trim)?;
            
            let mut iter = TokenIter::new(tokens.iter(), line_trim);

            if let Some(token) = iter.next() {

                match &token.kind {
                    TokenKind::Preprocessor(pre) => {
                        // skip

                        match pre {
                            PreprocessorKind::Line(tuple) => {
                                let mut last_set_name = if tuple.1.is_some() { tuple.1.clone().unwrap() } else { "".to_string() };

                                let _ = iter.iter.skip_while(|token| match &token.kind {
                                    TokenKind::Preprocessor(PreprocessorKind::Line(tuple)) => {
                                        if let Some(name) = &tuple.1 {
                                            last_set_name = name.to_string();
                                        }
                                        true
                                    },
                                    _ => false
                                });

                                if !last_set_name.is_empty() {
                                    if current_last_set_name != last_set_name {
                                        current_last_set_name = last_set_name.clone();
                                        out.push_str(&format!("// INCLUDE \"{}\"\n", last_set_name));
                                    }
                                }
                            },
                            _ => {
                                out.push_str(line);
                                out.push('\n');
                            }
                        }
                    }
                    _ => {
                        out.push_str(line);
                        out.push('\n');
                    }
                }
            }
            
        } else {
            out.push_str(line);
            out.push('\n');
        }
    }

    let lines = out.split('\n').collect::<Vec<&str>>();
    let mut last_out = String::new();

    let mut depth = 0;
    
    for line in lines {
        
        if line.starts_with("#ifdef 1") || line.starts_with("#if 1") || line.starts_with("#ifndef 0") {
            depth += 1;
        } else if line.starts_with("#endif") {
            depth -= 1;
        }

        if depth > 0 {
            continue;
        }

        last_out.push_str(line);
        last_out.push('\n');
    }

    Ok(last_out)
}

struct ModuleInfo {
    index: usize,
    is_main: bool,
    implements: Vec<String>,
    imports: Vec<Token>,
    lines: String,
    tokens: Vec<Token>,
}

fn link_stage(modules: Vec<String>) -> Result<String, String> {

    let mut mod_infos = Vec::new();

    let mut impls = HashSet::new();

    let mut index = 0;
    for module in modules {
        let tokens = tokenize(&module)?;
        let mut mod_info = ModuleInfo {
            index: index,
            is_main: false,
            implements: Vec::new(),
            imports: Vec::new(),
            lines: module.clone(),
            tokens: tokens,
        };
        
        let tokens = &mod_info.tokens;
        let mut iter = TokenIter::new(tokens.iter(), &module);
        while let Some(token) = iter.next() {
            match &token.kind {
                TokenKind::Identifier(identifier) => {
                    match identifier.as_str() {
                        "MainVS" | "MainPS" | "MainCS" | "MainGS" => {
                            mod_info.is_main = true;
                        },
                        _ => {}
                    }
                },
                TokenKind::Import => {
                    let path = iter.expect_literal(None, Some("Expected import path"))?;
                    
                    mod_info.imports.push(path.clone());
                },
                TokenKind::Implements => {
                    let path = iter.expect_literal(None, Some("Expected import path"))?;
                    
                    if impls.contains(&path.to_string()) {
                        return Err(format_error("", &path, &format!("Multiple linked modules implement {}", path.to_string())));
                    }
                    impls.insert(path.to_string());
                    mod_info.implements.push(path.to_string());
                },
                _ => {}
            }
        }
        
        mod_infos.push(mod_info);
        index += 1;
    }

    let mut main_mod: Option<&ModuleInfo> = None;
    for mod_info in &mod_infos {
        if mod_info.is_main {
            if main_mod.is_some() {
                return Err(format!("Multiple main modules found: {} and {}", main_mod.unwrap().tokens.first().unwrap().path.to_string(), mod_info.tokens.first().unwrap().path.to_string()));
            }

            main_mod = Some(mod_info);
        }
        
        for import in &mod_info.imports {
            if !impls.contains(&import.to_string()) {
                return Err(format_error("", &import, &format!("Imported module {} not implemented", import.to_string())));
            }
        }
    }

    if main_mod.is_none() {
        return Err("No main module found".to_string());
    }

    let mut included = HashSet::new();
    included.insert(main_mod.unwrap().tokens.first().unwrap().path.to_string());

    let lines = main_mod.unwrap().lines.split_terminator('\n').collect::<Vec<&str>>();

    let mut linked = String::new();

    for line in lines {
        let trimmed = line.trim();
        
        if trimmed.starts_with("@import") {
            let path = trimmed.split_whitespace().nth(1).unwrap().trim_matches('"').to_string();
            
            if !included.contains(&path) {
                included.insert(path.clone());

                for mod_info in &mod_infos {
                    if mod_info.implements.contains(&path) {
                        linked.push_str(mod_info.lines.split_terminator('\n').filter(|line| 
                            if !line.trim().is_empty() {
                                let trimmed = line.trim();
                                if trimmed.starts_with("@import") || trimmed.starts_with("@implements") {
                                    return false;
                                }
                                return true;
                            } else { return false; }
                        ).collect::<Vec<&str>>().join("\n").as_str());
                        linked.push('\n');
                        break;
                    }
                }
                
            }
        } else if trimmed.starts_with("@implements") {
            // Do nothing
        }
        else {
            linked.push_str(line);
            linked.push('\n');
        }
    }

    Ok(linked)
}

fn get_type(tokens: &Vec<Token>, lines: &str) -> Result<String, String> {
    let mut iter = TokenIter::new(tokens.iter(), lines);
    
    iter.skip_while(TokenKind::Type);
    let type_token = iter.expect_literal(None, Some("Expected type name"))?;

    iter.expect_terminator(Some('\n'), Some("Expected new line after type name"))?;

    Ok(iter.next().unwrap().to_string())
}

fn stage_1_compile(path: &std::path::PathBuf, include_paths: &Vec<&str>, defines: HashSet<String>) -> Result<ParseResult, String> {
    let src = preprocess_old(path, include_paths);
    if src.is_err() {
        return Err(src.unwrap_err());
    }
    let src = src.unwrap();
    let tokens = tokenize(src.as_str());
    if tokens.is_err() {
        return Err(tokens.unwrap_err());
    }
    let tokens = tokens.unwrap();
    let result = parse_stage1(tokens, src.as_str());
    if result.is_err() {
        return Err(result.unwrap_err());
    }
    let result = result.unwrap();
    Ok(result)
}

fn main() {

//     let src = "export void+MaterialPS(float2 uv)
// {
//     return float4(1.0, 0.0, 0.0, 1.0);
// }";


    let path = "/home/dz/Documents/Dev/HeliosEngine/HeliosEditor/Shaders/RenderPass/Forward.hsl";
    let path_material = "/home/dz/Documents/Dev/HeliosEngine/HeliosEditor/Shaders/Material/StandardSurface.hsl";
    let include_paths = vec!["/home/dz/Documents/Dev/HeliosEngine/HeliosEditor/Shaders"]; 

    let mut defines = HashSet::new();

    defines.insert("PIXEL".to_string());

    let result = stage_1_compile(&std::path::PathBuf::from(path), &include_paths, defines);
    let result_material = stage_1_compile(&std::path::PathBuf::from(path_material), &include_paths, HashSet::new());

    if result.is_err() {
        let err = result.unwrap_err();
        println!("{}", err.as_str());
        return;
    }

    if result_material.is_err() {
        let err = result_material.unwrap_err();
        println!("{}", err.as_str());
        return;
    }

    
    let src = result.unwrap().src;
    let src_material = result_material.unwrap().src;
    
    // println!("Src: \n{}", src);

    let link = link_stage(vec![src, src_material]);

    println!("Linked stage: {}", link.is_ok());
    
    if link.is_err() {
        let err = link.unwrap_err();
        println!("{}", err.as_str());
        return;
    } else {
        let result = link.unwrap();
        // println!("Linked stage: \n{}", result);

        let result = parse_stage2_late(&result, GraphicsAPI::Vulkan);

        // println!("Parsed stage 2: {}", result.is_ok());

        if result.is_err() {
            let err = result.unwrap_err();
            println!("{}", err.as_str());
        } else {
            let result = result.unwrap();
            println!("{}", result);
        }
    }
}
