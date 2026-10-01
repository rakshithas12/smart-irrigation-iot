import express from 'express';
import cors from 'cors';
import dotenv from 'dotenv';
dotenv.config();
const app=express(); app.use(cors()); app.use(express.json());
const PORT=process.env.PORT||5000, LAT=Number(process.env.LATITUDE||12.9716), LON=Number(process.env.LONGITUDE||77.5946), LOCATION=process.env.LOCATION_NAME||'Bengaluru';
let mode='AUTO';
let latest={soilMoisture:42,temperature:28.4,humidity:64,pump:false,source:'mock',timestamp:new Date().toISOString()};
let history=Array.from({length:20},(_,i)=>({timestamp:new Date(Date.now()-(19-i)*60000).toISOString(),soilMoisture:Math.round(35+Math.random()*30),temperature:Number((26+Math.random()*5).toFixed(1)),humidity:Math.round(55+Math.random()*20),pump:false}));
const rec=()=>latest.soilMoisture<30?{status:'IRRIGATION REQUIRED',message:'Soil is dry. The pump should run in AUTO mode.',level:'warning'}:latest.soilMoisture<45?{status:'MONITOR',message:'Soil moisture is getting low. Monitor the field.',level:'info'}:{status:'SOIL MOISTURE OK',message:'No irrigation is required at the moment.',level:'success'};
function add(){history.push(latest);if(history.length>60)history.shift()}
function mock(){let soil=Math.max(15,Math.min(85,latest.soilMoisture+(Math.random()-.5)*4));let temp=28+Math.sin(Date.now()/120000)*2+(Math.random()-.5);let hum=62+(Math.random()-.5)*8;latest={soilMoisture:Math.round(soil),temperature:Number(temp.toFixed(1)),humidity:Math.round(hum),pump:mode==='AUTO'?soil<30:latest.pump,source:'mock',timestamp:new Date().toISOString()};add()}
setInterval(mock,5000);
app.get('/api/health',(q,r)=>r.json({ok:true,time:new Date().toISOString()}));
app.get('/api/sensors',(q,r)=>r.json(latest));
app.post('/api/sensors',(q,r)=>{let {soilMoisture,temperature,humidity,pump}=q.body;if([soilMoisture,temperature,humidity].some(v=>typeof v!=='number'))return r.status(400).json({error:'soilMoisture, temperature and humidity must be numbers.'});latest={soilMoisture,temperature,humidity,pump:typeof pump==='boolean'?pump:latest.pump,source:'hardware',timestamp:new Date().toISOString()};add();r.json({success:true,data:latest})});
app.get('/api/history',(q,r)=>r.json(history));
app.get('/api/dashboard',(q,r)=>r.json({sensors:latest,mode,recommendation:rec(),location:LOCATION}));
app.get('/api/irrigation/recommendation',(q,r)=>r.json(rec()));
app.get('/api/pump/status',(q,r)=>r.json({pump:latest.pump,mode}));
app.post('/api/pump/control',(q,r)=>{if(mode!=='MANUAL')return r.status(400).json({error:'Switch to MANUAL mode before controlling the pump.'});if(typeof q.body.state!=='boolean')return r.status(400).json({error:'state must be boolean.'});latest={...latest,pump:q.body.state,timestamp:new Date().toISOString()};add();r.json({success:true,pump:latest.pump})});
app.post('/api/mode',(q,r)=>{let m=String(q.body.mode||'').toUpperCase();if(!['AUTO','MANUAL'].includes(m))return r.status(400).json({error:'mode must be AUTO or MANUAL.'});mode=m;if(mode==='AUTO')latest={...latest,pump:latest.soilMoisture<30};r.json({success:true,mode})});
app.get('/api/weather',async(q,r)=>{try{let u=new URL('https://api.open-meteo.com/v1/forecast');u.searchParams.set('latitude',LAT);u.searchParams.set('longitude',LON);u.searchParams.set('current','temperature_2m,relative_humidity_2m,precipitation,weather_code,wind_speed_10m');u.searchParams.set('daily','temperature_2m_max,temperature_2m_min,precipitation_probability_max,weather_code');u.searchParams.set('timezone','auto');u.searchParams.set('forecast_days','5');let x=await fetch(u);if(!x.ok)throw Error('Weather API error');let d=await x.json();r.json({location:LOCATION,current:d.current,daily:d.daily,source:'Open-Meteo'})}catch(e){r.status(503).json({error:'Weather service unavailable'})}});
app.listen(PORT,()=>console.log(`Backend: http://localhost:${PORT}`));
