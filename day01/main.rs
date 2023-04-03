use std::fs;

fn solve(nums: &Vec<u32>, offset: usize) -> u32 {
    let mut solution: u32 = 0;
    for i in 0..nums.len() {
        if nums[i] == nums[(i + offset) % nums.len()] {
            solution += nums[i];
        }
    }
    return solution;
}

fn main() {
    let input = fs::read_to_string("day01/input.txt").unwrap();
    let nums = input
        .chars()
        .flat_map(|c| c.to_digit(10))
        .collect::<Vec<u32>>();

    let part1 = solve(&nums, 1);
    let part2 = solve(&nums, nums.len() / 2);
    println!("{}", part1);
    println!("{}", part2);
}
