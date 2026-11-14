// Independent schema1 byte oracle: no C++ encoder or decoder calls.
function vectors(rules) {
  const cases = [
    [0n,1,3,2,0,0,0,0], [18446744073709551615n,6,1,0,3,1,2,1],
    [0x0123456789abcdefn,2,2,3,2,2,0,0], [1n,6,3,1,4294967292,1,0,1]
  ];
  return cases.map(([seed,number,hp,reserve,debt,a,b,pulse]) => {
    const bytes=[67,67,67,80];
    const le=(n,w)=>{n=BigInt(n);for(let i=0;i<w;i++,n>>=8n)bytes.push(Number(n&255n));};
    le(1,4);le(rules,4);le(seed,8);bytes.push(number,hp,reserve);
    le(debt,4);bytes.push(a,b,pulse,0,0);
    let checksum=14695981039346656037n;
    for(const byte of bytes)checksum=BigInt.asUintN(64,(checksum^BigInt(byte))*1099511628211n);
    le(checksum,8);
    return bytes;
  });
}
if(require.main===module)console.log(JSON.stringify(vectors(Number(process.argv[2]??9))));
module.exports={vectors};
