// Independent byte oracle for canonical hash v7. No C++ or resolver calls.
const minimal = () => ({ports: Array(49).fill(0), rot: Array(49).fill(0),
  blocked: Array(49).fill(0), player: 0, hp: 3, turns: 0, packet: [],
  powered: Array(49).fill(0), receivers: Array(49).fill(0), entry: [], exit: [],
  pulses: [], reserve: 2, debt: 0, modules: [0, 0], used: 0, emitters: [], capacitors: []});
function hash(s, version) {
  const bytes = [];
  const le = (n, width) => { n = BigInt(n); for (let i=0;i<width;i++,n>>=8n) bytes.push(Number(n&255n)); };
  le(version,4);
  bytes.push(...s.ports,...s.rot,...s.blocked,s.player,s.hp);
  le(s.turns,4); bytes.push(s.packet.length ? 1 : 0,...s.packet,
    ...s.powered,...s.receivers,s.entry.length ? 1 : 0,...s.entry,
    s.exit.length ? 1 : 0,...s.exit);
  le(s.pulses.length,8);
  for (const [id,pos] of s.pulses) { le(id,4); bytes.push(pos); }
  bytes.push(s.reserve); le(s.debt,4); bytes.push(...s.modules,s.used);
  if (version >= 5) {
    le(s.emitters.length,8);
    for (const [id,...fields] of s.emitters) { le(id,4); bytes.push(...fields); }
  }
  if (version >= 6) {
    le(s.capacitors.length,8);
    for (const [id,...fields] of s.capacitors) { le(id,4); bytes.push(...fields); }
  }
  if (version >= 7) bytes.push(s.requirement ?? 2);
  let h=14695981039346656037n;
  for (const b of bytes) h=BigInt.asUintN(64,(h^BigInt(b))*1099511628211n);
  return h.toString();
}
const vectors = [];
const migration = {};
const migrate = s => { const next=hash(s,7); migration[hash(s,6)]=next; return next; };
const add = s => vectors.push([hash(s,6),migrate(s)]);
add(minimal());
for (const reserve of [0,1,3,255]) add({...minimal(),reserve});
for (const debt of [1,4294967295]) add({...minimal(),debt});
for (const used of [0,1]) for (const modules of [[0,0],[1,0],[2,0],[1,2]]) add({...minimal(),used,modules});
const full = {...minimal(),player:48,hp:255,turns:4294967295,packet:[0,48],entry:[0],exit:[48],pulses:[[4294967295,48],[0,0]]};
for (let i=0;i<49;i++) {
  full.ports[i]=i%16; full.rot[i]=+(i%2===0); full.blocked[i]=+(i%3===0);
  full.powered[i]=+(i%5===0); full.receivers[i]=+(i%7===0);
}
add(full);
function fixture(mask) {
  const s=minimal();
  s.ports[0]=6;s.ports[1]=mask;s.ports[7]=3;s.ports[8]=9;
  s.rot[1]=1;s.receivers[1]=s.receivers[8]=1;s.entry=[0];s.exit=[8];s.pulses=[[1,7]];
  return s;
}
for (const mask of [6,9,3,12]) add(fixture(mask));
const snapshot = (mask,turns,packet,powered,player=0,reserve=3,debt=0) => {
  const s=fixture(mask); Object.assign(s,{turns,packet,hp:2,pulses:[],player,reserve,debt});
  for (const p of powered) s.powered[p]=1;
  add(s);
};
snapshot(12,1,[],[1]); snapshot(12,2,[1,8],[1]);
snapshot(12,3,[],[1,8]); snapshot(12,4,[],[1,8],1); snapshot(12,5,[],[1,8],8);
snapshot(9,1,[],[1],0,2,1); snapshot(6,2,[7,8],[1],0,1,2);
snapshot(9,3,[],[1,8],0,1,3); snapshot(6,4,[],[1,8],0,0,4);
const emitter = {...minimal(),emitters:[[0,0,2,0,1],[4294967295,48,48,47,48]]};
add(emitter);
const capacitor = {...minimal(),capacitors:[[0,0,1,1,1],[4294967295,48,48,48,48]]};
const capacitorHashes = {armed:migrate(capacitor)};
capacitor.capacitors[0][4]=0;capacitor.capacitors[1][4]=47;
capacitorHashes.pending=migrate(capacitor);
capacitor.capacitors[1][4]=0;
capacitorHashes.spent=migrate(capacitor);
// Explicit snapshots of the capacitor turn corpus, including a refused move.
const capacitorTrace=[];
for(const [bits,turns,remaining,reserve,debt] of [[10,0,2,2,0],[10,1,1,2,0],
  [10,1,1,2,0],[5,2,0,2,0],[10,3,0,2,0],[10,4,0,2,0],[10,5,0,1,1]]) {
  const s=minimal();s.ports[0]=2;s.ports[1]=bits;s.ports[2]=8;s.rot[1]=1;
  Object.assign(s,{turns,reserve,debt,capacitors:[[0,2,2,2,remaining]]});
  capacitorTrace.push(migrate(s));
}
const requirements = [hash(minimal(),7), hash({...minimal(),requirement:3},7)];
migrate(emitter);
if (require.main === module) {
  console.log(JSON.stringify({vectors,emitter:hash(emitter,7),capacitor:capacitorHashes,capacitorTrace,requirements,migration},null,2));
}
module.exports = {vectors, emitter:hash(emitter,7), capacitor:capacitorHashes,capacitorTrace,requirements,migration};
