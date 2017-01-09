

// http://stjarnhimlen.se/comp/ppcomp.html#0
/*
The primary orbital elements are here denoted as:
    N = longitude of the ascending node
    i = inclination to the ecliptic (plane of the Earth's orbit)
    w = argument of perihelion
    a = semi-major axis, or mean distance from Sun
    e = eccentricity (0=circle, 0-1=ellipse, 1=parabola)
    M = mean anomaly (0 at perihelion; increases uniformly with time)
Related orbital elements are:
    w1 = N + w   = longitude of perihelion
    L  = M + w1  = mean longitude
    q  = a*(1-e) = perihelion distance
    Q  = a*(1+e) = aphelion distance
    P  = a ^ 1.5 = orbital period (years if a is in AU, astronomical units)
    T  = Epoch_of_M - (M(deg)/360_deg) / P  = time of perihelion
    v  = true anomaly (angle between position and perihelion)
    E  = eccentric anomaly
*/	


function normalize(x) {
	while (x >= Math.PI*2) x -= Math.PI*2;
	while (x < 0) x += Math.PI*2;
	return x;
}

function RadToHour(x) {
	var t = normalize(x) / Math.PI * 12;
	var h = Math.floor(t);
	var m = Math.floor((t-h)*60);
	var s = Math.floor((t-h-m/60)*3600);
	return (h<10 ? "0" : "") + h + ":" + (m<10 ? "0" : "") + m + ":" + (s<10 ? "0" : "") + s + "";
}


var Astro = function(dateOb, longitude, latitude) 
{
	function CalcDay() {
		var y = dateOb.getUTCFullYear();
		var m = dateOb.getUTCMonth()+1;
		var D = dateOb.getUTCDate();
		var UT = dateOb.getUTCHours() + dateOb.getUTCMinutes()/60 + dateOb.getUTCSeconds()/3600;
		return 367*y - Math.floor(7 * ( y + Math.floor((m+9)/12) ) / 4) + Math.floor(275*m/9) + D - 730530 + UT/24;
	}
	this.offset = -dateOb.getTimezoneOffset() / 60 / 12 * Math.PI;
	this.longitude = longitude * Math.PI/180;
	this.latitude = latitude * Math.PI/180;
	this.dateOb = dateOb;
	this.UT = (dateOb.getUTCHours() + dateOb.getUTCMinutes()/60 + dateOb.getUTCSeconds()/3600) * Math.PI/12;
	this.d = CalcDay(); // Days since 2000.0

	var sin = Math.sin;
	var acos = Math.acos;
	var cos = Math.cos;
	var atan2 = Math.atan2;
	var sqrt = Math.sqrt;

	var ecl = (23.4393 - 3.563E-7 * this.d) * Math.PI/180;

	this.CalculateSun = function()
	{
		// Position of the Sun
		{
			this.sun.M = normalize(this.sun.M);
			// eccentric anomaly
			var E = this.sun.M + this.sun.e * sin(this.sun.M) * (1 + this.sun.e * cos(this.sun.M));

			// distance r, true anomaly v
			var xv = cos(E) - this.sun.e;
			var yv = sqrt(1-this.sun.e*this.sun.e) * sin(E);
			var v = atan2(yv, xv);
			var r = sqrt(xv*xv+yv*yv);
			
			// true longitude
			var L = (v + this.sun.w);
			// ecliptic coords
			var xs = r * cos(L);
			var ys = r * sin(L);
			// equatorial coords
			var xe = xs;
			var ye = ys * cos(ecl);
			var ze = ys * sin(ecl);
			// ra/dec
			this.sun.ra = normalize(atan2(ye,xe));
			this.sun.dec = (atan2(ze, sqrt(xe*xe+ye*ye)));

			this.sun.L = L;
			this.sun.r = r;
			this.sun.v = v;
		}

		// time calcs
		{
			var Ls = this.sun.v + this.sun.w;
			var GMST0 = Ls + Math.PI;
			// greenwich sidereal time
			this.GMST = normalize(GMST0 + this.UT);
			// local sidereal time
			this.LST = normalize(this.GMST + this.longitude);
			// local hour angle
			this.LHA = normalize(this.LST - this.sun.ra);


			// sun's altitude
			//var h = asin( sin(latitude) * sin(this.sun.dec) + cos(latitude) * cos(this.sun.dec) * cos(this.LHA));
			var h = 0;

			this.noon = normalize(this.sun.ra - GMST0 - this.longitude);
			var coslha = (sin(h) - sin(this.latitude)*sin(this.sun.dec)) / (cos(this.latitude) * cos(this.sun.dec));

			if (coslha < -1) { // midnight sun
				this.sunrise = 0;
				this.sunset = Math.PI*2;
			} else if (coslha > 1) { // seasonal night
				this.sunrise = this.noon;
				this.sunset = this.noon;
			} else {
				var hlha = acos(coslha);
				this.sunrise = this.noon - hlha;
				this.sunset = this.noon + hlha;
			}
		}
	};

	this.CalculateBody = function(body, geocentric)
	{
		// Position of planet
		{
		//	body.M = normalize(body.M);
			var N = body.N;
			var i = body.i;
			var w = body.w;
			var a = body.a;
			var e = body.e;
			var M = body.M;

			// eccentric anomaly
			var E = M + e * sin(M) * (1 + e * cos(M));

			// distance r, true anomaly v
			var xv = a * (cos(E) - e);
			var yv = a * (sqrt(1-e*e) * sin(E));
			var v = atan2(yv, xv);
			var r = sqrt(xv*xv+yv*yv);

			// heliocentric coordinates in AU (or geocentric in earth radii for moon)
			var xh = r * (cos(N) * cos(v+w) - sin(N) * sin(v+w) * cos(i));
			var yh = r * (sin(N) * cos(v+w) + cos(N) * sin(v+w) * cos(i));
			var zh = r * (sin(v+w) * sin(i));
		

			// geocentric coordinates
			var xg = xh;
			var yg = yh;
			var zg = zh;
			if (!geocentric) {
				var xs = this.sun.r * cos(this.sun.v + this.sun.w);
				var ys = this.sun.r * sin(this.sun.v + this.sun.w);
				xg = xh + xs;
				yg = yh + ys;
				zg = zh;
			}
			
			var lon = atan2(yh,xh);
			var lat = atan2(zh, sqrt(xh*xh+yh*yh));

			// equatorial coords
			var xe = xg;
			var ye = yg * cos(ecl) - zg * sin(ecl);
			var ze = yg * sin(ecl) + zg * cos(ecl);

			// ra/dec
			body.ra = normalize(atan2(ye,xe));
			body.dec = (atan2(ze, sqrt(xe*xe+ye*ye)));

			// geocentric distance
			var R = sqrt(xg*xg+yg*yg+zg*zg);

			// phase
			var FV = 0;
			if (!geocentric) {
				var s = this.sun.r;
				FV = acos( (r*r + R*R - s*s) / (2 * r * R));
			} else {
				FV = Math.PI - acos( cos(this.sun.L - lon) * cos(lat) );
				if (FV >= Math.PI/4 && FV < 3*Math.PI/4) body.sym = '\u25d1'; // half
				else if (FV >= 3*Math.PI/4 && FV < 5*Math.PI/4) body.sym = '\u25cb'; // full
				else if (FV >= 5*Math.PI/4 && FV < 7*Math.PI/4) body.sym = '\u25d0'; // half
				else body.sym = '\u25cf'; // new
			}		
			body.phase = (1 + cos(FV))/2;	
		}
	};

		

	this.toString = function() {
		var localstr = "UTC" + (this.dateOb.getTimezoneOffset()>0?"-":"+") + this.dateOb.getTimezoneOffset()/60;
		return "Date: " + this.dateOb.toString() + "\n" +
			"GMST       = " + RadToHour(this.GMST) + "\n" +
			"LST        = " + RadToHour(this.LST) + "\n" +
			"sunrise    = " + RadToHour(this.sunrise + this.offset) + " " + localstr + "\n" +
			"solar noon = " + RadToHour(this.noon + this.offset) + " " + localstr + "\n" +
			"sunset     = " + RadToHour(this.sunset + this.offset) + " " + localstr + "\n" +
			"d-2000.0   = " + Math.floor(this.d) + "\n" +
			"sun     = " + this.sun.toString() + "\n" +
			"moon    = " + this.moon.toString() +"\n" +
			"mercury = " + this.mercury.toString() +"\n" +
			"venus   = " + this.venus.toString() +"\n" +
			"mars    = " + this.mars.toString() +"\n" +
			"jupiter = " + this.jupiter.toString() +"\n" +
			"saturn  = " + this.saturn.toString() +"\n" +
			"";
	};

	// ORBITAL ELEMENTS
	this.sun = {
		N : (0.0)*Math.PI/180,
		i : (0.0)*Math.PI/180,
		w : (282.9404 + 4.70935E-5 * this.d)*Math.PI/180,
		a : 1.000000 ,
		e : 0.016709 - 1.151E-9 * this.d,
		M : (356.0470 + 0.9856002585 * this.d)*Math.PI/180,
		sym : '\u2609'
	};
	this.moon = {
		N : (125.1228 - 0.0529538083 * this.d)*Math.PI/180,
		i : (5.1454)*Math.PI/180,
		w : (318.0634 + 0.1643573223 * this.d)*Math.PI/180,
		a : 60.2666,
		e : 0.054900,
		M : (115.3654 + 13.0649929509 * this.d)*Math.PI/180,
		sym : '\u263d'
	};
	this.mercury = {
		N : (48.3313 + 3.24587E-5 * this.d)*Math.PI/180,
		i : (7.0047 + 5.00E-8 * this.d)*Math.PI/180,
		w : (29.1241 + 1.01444E-5 * this.d)*Math.PI/180,
		a : 0.387098 ,
		e : 0.205635 + 5.59E-10 * this.d,
		M : (168.6562 + 4.0923344368 * this.d)*Math.PI/180,
		sym : '\u263f'
	};
	this.venus = {
		N : (76.6799 + 2.46590E-5 * this.d)*Math.PI/180,
		i : (3.3946 + 2.75E-8 * this.d)*Math.PI/180,
		w : (54.8910 + 1.38374E-5 * this.d)*Math.PI/180,
		a : 0.723330 ,
		e : 0.006773 - 1.302E-9 * this.d,
		M : (48.0052 + 1.6021302244 * this.d)*Math.PI/180,
		sym : '\u2640'
	};
	this.mars = {
		N : (49.5574 + 2.11081E-5 * this.d)*Math.PI/180,
		i : (1.8497 - 1.78E-8 * this.d)*Math.PI/180,
		w : (286.5016 + 2.92961E-5 * this.d)*Math.PI/180,
		a : 1.523688 ,
		e : 0.093405 + 2.516E-9 * this.d,
		M : (18.6021 + 0.5240207766 * this.d)*Math.PI/180,
		sym : '\u2642'
	};
	this.jupiter = {
		N : (100.4542 + 2.76854E-5 * this.d)*Math.PI/180,
		i : (1.3030 - 1.557E-7 * this.d)*Math.PI/180,
		w : (273.8777 + 1.64505E-5 * this.d)*Math.PI/180,
		a : 5.20256 ,
		e : 0.048498 + 4.469E-9 * this.d,
		M : (19.8950 + 0.0830853001 * this.d)*Math.PI/180,
		sym : '\u2643'
	};
	this.saturn = {
		N : (113.6634 + 2.38980E-5 * this.d)*Math.PI/180,
		i : (2.4886 - 1.081E-7 * this.d)*Math.PI/180,
		w : (339.3939 + 2.97661E-5 * this.d)*Math.PI/180,
		a : 9.55475 ,
		e : 0.055546 - 9.499E-9 * this.d,
		M : (316.9670 + 0.0334442282 * this.d)*Math.PI/180,
		sym : '\u2644'
	};

	this.bodies = [ this.sun, this.moon, this.mercury, this.venus, this.mars, this.jupiter, this.saturn ];

	var printBody = function() {
		var phaseStr = !isNaN(this.phase) ? (", " + Math.floor(this.phase*100) + "% illuminated") : "";
		var dec = ""+Math.floor(this.dec*180/Math.PI*100)/100;
	    while (dec.length < 6) dec = " " + dec;
		return RadToHour(this.ra) + " / " + 
							dec + "\u00b0" + phaseStr;
	};

	for (var i = 0; i < this.bodies.length; i++) this.bodies[i].toString = printBody;

	this.CalculateSun();
	this.CalculateBody(this.mercury, false);
	this.CalculateBody(this.venus, false);
	this.CalculateBody(this.mars, false);
	this.CalculateBody(this.jupiter, false);
	this.CalculateBody(this.saturn, false);
	this.CalculateBody(this.moon, true);
};


/////////////////////////////////////////

module.exports.sunrise = 0;
module.exports.sunset = 0;
module.exports.sun = 0;
module.exports.moon = 0;
module.exports.mercury = 0;
module.exports.venus = 0;
module.exports.mars = 0;
module.exports.jupiter = 0;
module.exports.saturn = 0;

function AngleToIndex(x) {
    return (Math.round(normalize(x) * 30 / Math.PI) + 30) % 60;
}

var astro;

module.exports.calculate = function(lat, lon, date) {
    astro = new Astro(date, lon, lat);
    
    console.log(astro);

    var sunAngle = normalize(astro.UT + astro.offset);
    var sunriseAngle = (astro.sunrise + astro.offset);
    var sunsetAngle = (astro.sunset + astro.offset);

    var raError = sunAngle + astro.sun.ra; // sun and clock are fixed, everything else moves
    var moonAngle = raError - astro.moon.ra;
    var mercuryAngle = raError - astro.mercury.ra;
    var venusAngle = raError - astro.venus.ra;
    var marsAngle = raError - astro.mars.ra;
    var jupiterAngle = raError - astro.jupiter.ra;
    var saturnAngle = raError - astro.saturn.ra;

    module.exports.sunrise = AngleToIndex(sunriseAngle);
    module.exports.sunset = AngleToIndex(sunsetAngle);
    module.exports.sun = AngleToIndex(sunAngle);
    module.exports.moon = AngleToIndex(moonAngle);
    module.exports.mercury = AngleToIndex(mercuryAngle);
    module.exports.venus = AngleToIndex(venusAngle);
    module.exports.mars = AngleToIndex(marsAngle);
    module.exports.jupiter = AngleToIndex(jupiterAngle);
    module.exports.saturn = AngleToIndex(saturnAngle);
};


