// Independent BigInt oracle; numeric indices, no C++ or game content.
const mask = (1n << 64n) - 1n;
function indices(count, seed, sector) {
  if (!count || count > 12) return [255, 255];
  if (count <= 2) return [0, count === 2 ? 1 : 255];
  let state = (seed ^ (BigInt(sector) * 0x9e3779b97f4a7c15n)) & mask;
  function draw(bound) {
    const k = BigInt(bound);
    let z;
    do {
      state = (state + 0x9e3779b97f4a7c15n) & mask;
      z = state;
      z = ((z ^ (z >> 30n)) * 0xbf58476d1ce4e5b9n) & mask;
      z = ((z ^ (z >> 27n)) * 0x94d049bb133111ebn) & mask;
      z ^= z >> 31n;
    } while (z < (1n << 64n) % k);
    return Number(z % k);
  }
  const a = draw(count);
  let b = draw(count - 1);
  if (b >= a) ++b;
  return [a, b].sort((x,y) => x-y);
}
// Module IDs from the admitted pool 1..12 (P085-19); 0 marks an empty slot.
function offer(owned, seed, sector) {
  if (owned.length >= 2) return [0, 0];
  const pool = [];
  for (let id = 1; id <= 12; ++id) if (!owned.includes(id)) pool.push(id);
  return indices(pool.length, seed, sector).map(i => i === 255 ? 0 : pool[i]);
}
const loadouts = [[], [1], [2], [1, 2], [7], [12]];
if (process.argv[2] === 'offers') {
  let digest = 14695981039346656037n;
  for (const owned of loadouts)
    for (let seed=0n; seed<257n; ++seed)
      for (let sector=1; sector<=6; ++sector)
        for (const id of offer(owned, seed, sector))
          digest = ((digest ^ BigInt(id)) * 1099511628211n) & mask;
  const table = {};
  for (const seed of [0n, (1n << 64n) - 1n])
    for (const owned of loadouts.slice(0, 3))
      for (let sector=1; sector<=6; ++sector)
        table[`${seed}/${owned.join('+') || '-'}/${sector}`] = offer(owned, seed, sector);
  console.log(digest.toString());
  console.log(JSON.stringify(table));
} else {
  let digest = 14695981039346656037n;
  for (let count=0; count<=12; ++count)
    for (let seed=0n; seed<257n; ++seed)
      for (let sector=1; sector<=6; ++sector)
        for (const index of indices(count,seed,sector))
          digest = ((digest ^ BigInt(index)) * 1099511628211n) & mask;
  console.log(digest.toString());
}
