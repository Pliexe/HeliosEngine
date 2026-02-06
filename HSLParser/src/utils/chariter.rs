pub struct CharIter<'a> {
    // input: &'a str,
    iter: std::iter::Peekable<std::str::Chars<'a>>,
    pub(crate) line: usize,
    pub(crate) column: usize,
}

impl<'a> CharIter<'a> {
    pub fn new(input: &'a str) -> CharIter<'a> {
        CharIter { iter: input.chars().peekable(), line: 1, column: 0 }
    }

    pub fn peek(&mut self) -> Option<&char> {
        self.iter.peek()
    }

    pub fn collect_while<F>(&mut self, condition: F) -> String where F: Fn(char) -> bool {
        let mut s = String::new();
        while let Some(c) = self.peek() {
            if condition(*c) {
                s.push(self.next().unwrap());
            } else {
                break;
            }
        }
        s
    }

    pub fn skip_while(&mut self, condition: fn(char) -> bool) -> &mut CharIter<'a> {
        while let Some(c) = self.peek() {
            if condition(*c) {
                self.next();
            } else {
                break;
            }
        }
        self
    }

    pub fn collect_while_current_and_previous(&mut self, prev_condition: fn(char) -> bool, condition: fn(char) -> bool) -> String {
        let mut s = String::new();
        let mut prev = '\0';
        while let Some(c) = self.peek() {
            if condition(*c) && prev_condition(prev) {
                s.push(*c);
                prev = *c;
                self.next();
            } else {
                break;
            }
        }
        s
    }

    pub fn collect_while_not_escaped(&mut self, condition: fn(char) -> bool) -> String {
        let mut s = String::new();
        let mut escaped = false;

        print!("COLLECT: ");

        while let Some(c) = self.peek() {
            if condition(*c) || escaped {
                escaped = !escaped && *c == '\\'; // if the previous character was not escape character then attempt to escape
                // print!("{} {} | ", c, if escaped { 1 } else { 0 });
                if !escaped { s.push(*c); } // if not esacped, push the character
                self.next();
            } else {
                break;
            }
        }
        println!("\n");
        s
    }

    pub fn next_if(&mut self, condition: fn(char) -> bool) -> Option<char> {
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

    pub fn next(&mut self) -> Option<char> {
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