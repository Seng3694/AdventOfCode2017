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

    let mut part2 = 0;
    for line in number_lines {
        for i in 0..line.len() {
            for j in 0..line.len() {
                if line[i] > line[j] && line[i] % line[j] == 0 {
                    part2 += line[i] / line[j]
                }
            }
        }
    }

    println!("{}", part1);
    println!("{}", part2);

    return Ok(());
}
