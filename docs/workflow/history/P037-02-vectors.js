// Independent specification calculation: Node Buffer LE and BigInt FNV.
const vectors = [
 [0n,1,3,2,0,0,0,false],
 [18446744073709551615n,3,1,0,3,1,2,true],
 [0x0123456789abcdefn,2,2,3,2,2,0,false],
 [1n,3,3,1,4294967292,1,0,true]
];
for (const [seed,number,hp,reserve,debt,m0,m1,pulse] of vectors) {
 const bytes=Buffer.alloc(40);
 bytes.write('CCCP',0,'ascii'); bytes.writeUInt32LE(1,4); bytes.writeUInt32LE(1,8);
 bytes.writeBigUInt64LE(seed,12); bytes[20]=number; bytes[21]=hp; bytes[22]=reserve;
 bytes.writeUInt32LE(debt,23); bytes[27]=m0; bytes[28]=m1; bytes[29]=Number(pulse);
 let hash=14695981039346656037n;
 for(const byte of bytes.subarray(0,32)) hash=BigInt.asUintN(64,(hash^BigInt(byte))*1099511628211n);
 bytes.writeBigUInt64LE(hash,32);
 console.log(String(seed),String(hash),Array.from(bytes).join(','));
}
