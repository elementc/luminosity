

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

function RadToHourInt(x){
    var t = normalize(x) / Math.PI * 12;
	var h = Math.floor(t);
	var m = Math.floor((t-h)*60);
    if (m > 30)
        h++;
    return h;
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


		

	this.toString = function() {
		var localstr = "UTC" + (this.dateOb.getTimezoneOffset()>0?"-":"+") + this.dateOb.getTimezoneOffset()/60;
		return "Date: " + this.dateOb.toString() + "\n" +
			"GMST       = " + RadToHour(this.GMST) + "\n" +
			"LST        = " + RadToHour(this.LST) + "\n" +
			"sunrise    = " + RadToHour(this.sunrise + this.offset) + " " + localstr + "\n" +
			"solar noon = " + RadToHour(this.noon + this.offset) + " " + localstr + "\n" +
			"sunset     = " + RadToHour(this.sunset + this.offset) + " " + localstr + "\n" +
			"d-2000.0   = " + Math.floor(this.d) + "\n" +
			"sun     = " + this.sun.toString() + "";
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

	this.bodies = [ this.sun];


	this.CalculateSun();
};


/////////////////////////////////////////

module.exports.sunrise = 0;
module.exports.sunset = 0;

var astro;

module.exports.calculate = function(lat, lon, date) {
    astro = new Astro(date, lon, lat);
    
    console.log(astro);

    var sunriseAngle = (astro.sunrise + astro.offset);
    var sunsetAngle = (astro.sunset + astro.offset);

    module.exports.sunrise = RadToHourInt(sunriseAngle);
    module.exports.sunset = RadToHourInt(sunsetAngle);

};


