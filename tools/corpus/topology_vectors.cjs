// Independent BigInt oracle for numerical normal topology vectors. No C++.
const mask64=(1n<<64n)-1n;
function rng(seed) {
 let state=BigInt(seed);
 return ()=>{
   state=(state+0x9e3779b97f4a7c15n)&mask64;
   let z=state;
   z=((z^(z>>30n))*0xbf58476d1ce4e5b9n)&mask64;
   z=((z^(z>>27n))*0x94d049bb133111ebn)&mask64;
   return z^(z>>31n);
 };
}
function topology(root,number) {
 const rootRng=rng(root);let seed;
 for(let i=0;i<number;i++)seed=rootRng();
 const next=rng(seed),top=next()%2n===0n;
 const at=i=>top?(i<=6?i:6+(i-6)*7):(i<=6?i*7:42+i-6);
 const direction=(a,b)=>b===a+1?2:b===a-1?8:b>a?4:1;
 const masks=[];
 for(let i=0;i<13;i++) {
   let bits=(i?direction(at(i),at(i-1)):0)|(i<12?direction(at(i),at(i+1)):0);
   if(![0,4,8,12].includes(i)) {
     const turns=Number(next()%4n);
     for(let j=0;j<turns;j++)bits=((bits<<1)&15)|(bits>>3);
   }
   masks.push(bits);
 }
 return {root:BigInt(root).toString(),number,top,masks};
}
if(require.main===module)for(const root of [0n,1n,2n,mask64])for(let n=4;n<=6;n++){
 const v=topology(root,n);console.log(`    {${root===mask64?'UINT64_MAX':root}, ${n}, ${v.top}, {${v.masks.join(', ')}}},`);
}
module.exports={topology};
