# 🧩 GameOfLifePoket — Game of Life Patterns & Examples

This file is a quick reference of classic **Conway’s Game of Life** patterns you can build by hand.

**Legend**
- `#` = alive cell
- `.` = dead cell

You can place each pattern anywhere on your screen/grid.

---

## ✅ Rules (Conway / B3/S23)

At each step (“generation”), count the 8 neighbors around a cell:

- **Birth (B3):** a dead cell becomes alive if it has **exactly 3** alive neighbors.
- **Survival (S23):** a living cell stays alive if it has **2 or 3** alive neighbors.
- Otherwise, the cell dies.

---

## 🧱 1) Still Lifes (Stable / Fixed)

### Block (2×2) — stable
```
.....
.##..
.##..
.....
.....
```

### Beehive — stable
```
......
..##..
.#..#.
..##..
......
```

### Boat — stable
```
.....
.##..
.#.#.
..#..
.....
```

---

## 🔁 2) Oscillators (Repeat in place)

### Blinker (period 2)
State A:
```
.....
..#..
..#..
..#..
.....
```

State B:
```
.....
.....
.###.
.....
.....
```

### Toad (period 2)
State A:
```
......
..###.
.###..
......
```

State B:
```
......
...#..
.#..#.
.#..#.
..#...
......
```

### Beacon (period 2)
```
......
.##...
.##...
...##.
...##.
......
```

---

## 🚀 3) Spaceships (Move across the grid)

### Glider (Planeur) — moves diagonally
```
.....
..#..
...#.
.###.
.....
```

### LWSS (Lightweight Spaceship) — moves horizontally
```
........
.#..#...
#....#..
#...##..
#####...
........
```

---

## 🧬 4) Methuselahs (Small start → long evolution)

### R-pentomino (famous)
```
.....
..##.
.##..
..#..
.....
```

### Acorn (very long evolution)
```
.........
...#.....
.....#...
..##..###
.........
```

### Diehard (eventually dies, but lasts long)
```
........
......#.
.##.....
..#...##
...###..
........
```

---

## 💥 5) Simple collision ideas

### Two gliders head-on (try different offsets!)
Glider A:
```
.....
..#..
...#.
.###.
.....
```

Glider B (mirrored direction):
```
.....
.###.
.#...
..#..
.....
```

Place them facing each other and watch what happens.

---

## ✅ Tips for small screens

- Start with **Block** and **Blinker** to confirm rules are correct.
- Then try **Glider** (best “wow” effect on tiny screens).
- Use **Acorn** or **R-pentomino** for long evolving demos.
