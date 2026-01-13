data color {
    r, g, b
}

fn loops() {
    i = 0;
    s1 = 0;

    loop {
        i += 1;

        if i % 2 == 0 {
            continue;
        }

        s1 += 10;

        if i >= 10 {
            break;
        }
    }

    return s1
}

fn conditions(num, threshold, higher, lower, same) {
    if num > threshold {
        return higher
    } elif num < threshold {
        return lower
    } else {
        return same
    }
}
        

fn sum(list) {
    s = 0;

    loop it in list {
        s += it;
    }

    return it;
}

fn add(a, b) {
    return a + b;
}

fn main() {
    a = 10 * 20;
    b = a + 20;
    c = 2 + a * 3 + b;
    d = 100 - a / 10 - b * (c + 3);
    e = d % 10 + 100;

    l = [a, b, c, d, e];
    s = {a, b, c, d, e};
    d = {
        0: a,
        1: b,
        2: c,
        3: d,
        4: e
    };

    x = sum(l) * (a + sum(b, c) * d) - e;

    print(a);
    print(b);
    print(c);
    print(d);
    print(e);
    print(l);
    print(s);
    print(d);
    print(x);

    c1 = color { .r = 10, .g = 20, .b = 30 };
    c2 = color { 10, 20, 30 };

    print(c1);
    print(c2);

    c3 = color { 
        .r = c1.r + c2.r,
        .g = c1.g * (c2.g + 10),
        .b = (c1.b + c2.b / 10)
    };

    r = loops();

    print(r);

    print(conditions(10, 3, "Higher", "Lower", "Same"));
    print(conditions(10, 30, "Higher", "Lower", "Same"));
}
