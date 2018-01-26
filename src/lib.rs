#[macro_use]
extern crate helix;

use std::cmp::{self, Ordering};

ruby! {
    class VersionSorter {
        def int_sort(versions: Vec<String>) -> Vec<u32> {
            do_sort(versions, cmp_fwd)
        }

        def int_rsort(versions: Vec<String>) -> Vec<u32> {
            do_sort(versions, cmp_bwd)
        }

        def compare(a: String, b: String) -> i32 {
            cmp_fwd(&parse(&a, 0), &parse(&b, 0)) as i32
        }
    }
}

struct VersionNumber<'a> {
    i: usize,
    comps: Vec<VersionComponent<'a>>,
}

enum VersionComponent<'a> {
    String(&'a [u8]),
    Number(u32),
}

fn cmp_fwd(a: &VersionNumber, b: &VersionNumber) -> Ordering {
    let max_n = cmp::min(a.comps.len(), b.comps.len());

    for n in 0..max_n {
        match (&a.comps[n], &b.comps[n]) {
            (&VersionComponent::Number(a), &VersionComponent::Number(b)) => match a.cmp(&b) {
                Ordering::Equal => (),
                r => return r,
            },
            (&VersionComponent::String(a), &VersionComponent::String(b)) => {
                let sz = cmp::min(a.len(), b.len());
                let a = &a[..sz];
                let b = &b[..sz];
                match a.cmp(&b) {
                    Ordering::Equal => (),
                    r => return r,
                }
            }
            (&VersionComponent::Number(_), _) => return Ordering::Greater,
            _ => return Ordering::Less,
        }
    }

    if a.comps.len() < b.comps.len() {
        match &b.comps[max_n] {
            &VersionComponent::Number(_) => Ordering::Less,
            _ => Ordering::Greater,
        }
    } else if a.comps.len() > b.comps.len() {
        match &a.comps[max_n] {
            &VersionComponent::Number(_) => Ordering::Greater,
            _ => Ordering::Less,
        }
    } else {
        Ordering::Equal
    }
}

fn cmp_bwd(a: &VersionNumber, b: &VersionNumber) -> Ordering {
    cmp_fwd(a, b).reverse()
}

fn do_sort<F>(versions: Vec<String>, cmp: F) -> Vec<u32>
where
    F: FnMut(&VersionNumber, &VersionNumber) -> Ordering,
{
    let len = versions.len();
    let mut vns = Vec::with_capacity(len);
    for i in 0..len {
        vns.push(parse(&versions[i], i));
    }
    vns.sort_unstable_by(cmp);

    let mut sorted_versions = Vec::with_capacity(len);
    for i in 0..len {
        sorted_versions.push(vns[i].i as u32);
    }

    sorted_versions
}

fn parse(v: &str, i: usize) -> VersionNumber {
    let v = v.as_bytes();
    let mut comps = vec![];

    let mut offset = 0;
    let len = v.len();
    while offset < len && comps.len() < 64 {
        if (v[offset] as char).is_digit(10) {
            let mut number: u32 = 0;
            let start = offset;
            let mut overflown = false;

            while offset < len && (v[offset] as char).is_digit(10) {
                let old_number = number;
                number = number
                    .wrapping_mul(10)
                    .wrapping_add((v[offset] - b'0') as u32);
                if number < old_number {
                    overflown = true;
                }

                offset += 1;
            }

            if overflown {
                comps.push(VersionComponent::String(&v[start..offset]));
            } else {
                comps.push(VersionComponent::Number(number));
            }
        } else if v[offset] == b'-' || (v[offset] as char).is_alphabetic() {
            let start = offset;

            if v[offset] == b'-' {
                offset += 1;
            }

            while offset < len && (v[offset] as char).is_alphabetic() {
                offset += 1;
            }

            comps.push(VersionComponent::String(&v[start..offset]));
        } else {
            offset += 1;
        }
    }

    VersionNumber { i: i, comps: comps }
}
