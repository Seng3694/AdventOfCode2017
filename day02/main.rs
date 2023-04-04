use std::{
    fs::File,
    io::{self, BufRead, BufReader},
};

fn main() -> io::Result<()> {
    let file = File::open("day02/input.txt")?;
    let lines = BufReader::new(file).lines();

    let number_lines = lines
        .map(|line| {
            line.unwrap()
                .split_whitespace()
                .map(|number| number.parse::<u32>().unwrap())
                .collect::<Vec<u32>>()
        })
        .collect::<Vec<Vec<u32>>>();

    let part1 = number_lines
        .iter()
        .map(|v| v.iter().max().unwrap() - v.iter().min().unwrap())
        .sum::<u32>();

    println!("{}", part1);
    return Ok(());
}
