
#[derive(Debug, Clone, PartialEq, Eq)]
pub enum OperatorKind {
    // arithmnentic
    Plus, // +
    Minus, // -
    Mul, // *
    Div, // /
    Mod, // %

    // comparison
    Eq, // ==
    NotEq, // !=
    Less, // <
    LessEq, // <=
    Greater, // >
    GreaterEq, // >=

    // logical
    And, // &&
    Or, // ||
    Not, // !

    // assignment
    Assign, // =
    AddAssign, // +=
    SubAssign, // -=
    MulAssign, // *=
    DivAssign, // /=
    ModAssign, // %=

    // bitwise
    BitAnd, // &
    BitOr, // |
    BitXor, // ^
    ShiftL, // <<
    ShiftR, // >>
    
    // incremnet / decrement
    Inc, // ++
    Dec, // --
    
    Unknown(String),
}

impl OperatorKind {
    pub fn to_string(&self) -> String { self.to_str().to_string() }
    pub fn to_str(&self) -> &str {
        match self {

            // arithmnentic
            OperatorKind::Plus => "+",
            OperatorKind::Minus => "-",
            OperatorKind::Mul => "*",
            OperatorKind::Div => "/",
            OperatorKind::Mod => "%",

            // comparison
            OperatorKind::Eq => "==",
            OperatorKind::NotEq => "!=",
            OperatorKind::Less => "<",
            OperatorKind::LessEq => "<=",
            OperatorKind::Greater => ">",
            OperatorKind::GreaterEq => ">=",

            // logical
            OperatorKind::And => "&&",
            OperatorKind::Or => "||",
            OperatorKind::Not => "!",

            // assignment
            OperatorKind::Assign => "=",
            OperatorKind::AddAssign => "+=",
            OperatorKind::SubAssign => "-=",
            OperatorKind::MulAssign => "*=",
            OperatorKind::DivAssign => "/=",
            OperatorKind::ModAssign => "%=",

            // bitwise
            OperatorKind::BitAnd => "&",
            OperatorKind::BitOr => "|",
            OperatorKind::BitXor => "^",
            OperatorKind::ShiftL => "<<",
            OperatorKind::ShiftR => ">>",

            // incremnet / decrement
            OperatorKind::Inc => "++",
            OperatorKind::Dec => "--",

            OperatorKind::Unknown(s) => s.as_str(),
        }
    }

    pub fn all() -> &'static [OperatorKind] {
        &[
            OperatorKind::Plus,
            OperatorKind::Minus,
            OperatorKind::Mul,
            OperatorKind::Div,
            OperatorKind::Mod,

            OperatorKind::Eq,
            OperatorKind::NotEq,
            OperatorKind::Less,
            OperatorKind::LessEq,
            OperatorKind::Greater,
            OperatorKind::GreaterEq,

            OperatorKind::And,
            OperatorKind::Or,
            OperatorKind::Not,

            OperatorKind::Assign,
            OperatorKind::AddAssign,
            OperatorKind::SubAssign,
            OperatorKind::MulAssign,
            OperatorKind::DivAssign,
            OperatorKind::ModAssign,

            OperatorKind::BitAnd,
            OperatorKind::BitOr,
            OperatorKind::BitXor,
            OperatorKind::ShiftL,
            OperatorKind::ShiftR,
            
            OperatorKind::Inc,
            OperatorKind::Dec,
        ]
    }

    pub fn from(s: &str) -> OperatorKind {
        match s {
            // arithmnentic
            "+" => OperatorKind::Plus,
            "-" => OperatorKind::Minus,
            "*" => OperatorKind::Mul,
            "/" => OperatorKind::Div,
            "%" => OperatorKind::Mod,

            // comparison
            "==" => OperatorKind::Eq,
            "!=" => OperatorKind::NotEq,
            "<" => OperatorKind::Less,
            "<=" => OperatorKind::LessEq,
            ">" => OperatorKind::Greater,
            ">=" => OperatorKind::GreaterEq,

            // logical
            "&&" => OperatorKind::And,
            "||" => OperatorKind::Or,
            "!" => OperatorKind::Not,

            // assignment
            "=" => OperatorKind::Assign,
            "+=" => OperatorKind::AddAssign,
            "-=" => OperatorKind::SubAssign,
            "*=" => OperatorKind::MulAssign,
            "/=" => OperatorKind::DivAssign,
            "%=" => OperatorKind::ModAssign,

            // bitwise
            "&" => OperatorKind::BitAnd,
            "|" => OperatorKind::BitOr,
            "^" => OperatorKind::BitXor,
            "<<" => OperatorKind::ShiftL,
            ">>" => OperatorKind::ShiftR,
    
            // incremnet / decrement
            "++" => OperatorKind::Inc,
            "--" => OperatorKind::Dec,
            _    => OperatorKind::Unknown(s.to_string()),
        }
    }

    pub fn length(&self) -> usize {
        self.to_str().len()
    }
}