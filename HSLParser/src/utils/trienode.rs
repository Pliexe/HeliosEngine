use std::collections::HashMap;

use crate::utils::chariter::CharIter;

pub struct TrieNode {
    children: HashMap<char, TrieNode>,
}

impl TrieNode {
    pub fn new() -> TrieNode {
        TrieNode { children: HashMap::new() }
    }

    pub fn insert(&mut self, key: char) {
        let node = self.children.entry(key).or_insert(TrieNode::new());
    }

    pub fn insert_str(&mut self, s: &str) {
        let mut node = self;
        for c in s.chars() {
            node = node.children.entry(c).or_insert(TrieNode::new());
        }
    }

    pub fn search(&self, key: char) -> Option<&TrieNode> {
        self.children.get(&key)
    }

    pub fn has(&self, key: char) -> bool {
        self.children.contains_key(&key)
    }

    pub fn collect(&self, iter: &mut CharIter) -> String {
        let mut v = String::new();

        let mut node = self;

        loop {
            let char = iter.peek();
            if char.is_none() {
                break;
            }

            if let Some(new_node) = node.search(*char.unwrap()) {
                node = new_node;
                v.push(iter.next().unwrap());
            } else {
                break;
            }
        }

        v
    }
}