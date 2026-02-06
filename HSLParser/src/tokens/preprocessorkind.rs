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