const wrap=n=>BigInt.asUintN(64,n);
function rng(seed){let state=seed;return()=>{state=wrap(state+0x9e3779b97f4a7c15n);let z=state;z=wrap((z^(z>>30n))*0xbf58476d1ce4e5b9n);z=wrap((z^(z>>27n))*0x94d049bb133111ebn);return z^(z>>31n);};}
for(const root of [0n,1n,2n,18446744073709551615n]){
 const seed=rng(root)(),draw=rng(seed),top=(draw()%2n)===0n;
 const path=Array.from({length:13},(_,i)=>top?(i<=6?i:6+(i-6)*7):(i<=6?i*7:42+i-6));
 const dir=(a,b)=>b===a+1?2:b===a-1?8:b>a?4:1;
 const masks=path.map((p,i)=>{let m=(i?dir(p,path[i-1]):0)|(i<12?dir(p,path[i+1]):0);if(![0,4,8,12].includes(i)){let n=Number(draw()%4n);while(n--)m=((m<<1)&15)|(m>>3);}return m;});
 console.log(String(root),String(seed),top,masks.join(','));
}
