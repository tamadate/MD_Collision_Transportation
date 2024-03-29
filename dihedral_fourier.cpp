#include "md.hpp"
/*########################################################################################

-----compute intramolecular interaction-----

#######################################################################################*/

/**********************************Force calculation******************************************/
void
MD::dihedral_fourier(void) {
	Ion *ions = vars->ions.data();
/*intra-molecular interaction (dihedral)*/
	double vb1x,vb1y,vb1z,vb2x,vb2y,vb2z,vb3x,vb3y,vb3z,vb2xm,vb2ym,vb2zm;
	double edihedral,ff2[3],ff4[3],ff1[3],ff3[3];
	double ax,ay,az,bx,by,bz,rasq,rbsq,rgsq,rg,rginv,ra2inv,rb2inv,rabinv;
	double df,df1_,ddf1_,fg,hg,fga,hgb,gaa,gbb;
	double dtfx,dtfy,dtfz,dtgx,dtgy,dtgz,dthx,dthy,dthz;
	double c,s,p_,sx2,sy2,sz2, m;
	Dihedral_type *dtypes = vars->dtypes.data();
	for (auto &d : vars-> dihedrals) {
		int i=d.atom1, j=d.atom2, k=d.atom3, l=d.atom4, type=d.type;
   //     cout<<i<<" "<<j<<" "<<k<<" "<<l<<" "<<endl;
		// 1st bond
		vb1x = ions[i].qx - ions[j].qx;
		vb1y = ions[i].qy - ions[j].qy;
		vb1z = ions[i].qz - ions[j].qz;
		// 2nd bond
		vb2x = ions[k].qx - ions[j].qx;
		vb2y = ions[k].qy - ions[j].qy;
		vb2z = ions[k].qz - ions[j].qz;
		vb2xm = -vb2x;
		vb2ym = -vb2y;
		vb2zm = -vb2z;
		// 3rd bond
		vb3x = ions[l].qx - ions[k].qx;
		vb3y = ions[l].qy - ions[k].qy;
		vb3z = ions[l].qz - ions[k].qz;

		ax = vb1y*vb2zm - vb1z*vb2ym;
		ay = vb1z*vb2xm - vb1x*vb2zm;
		az = vb1x*vb2ym - vb1y*vb2xm;
		bx = vb3y*vb2zm - vb3z*vb2ym;
		by = vb3z*vb2xm - vb3x*vb2zm;
		bz = vb3x*vb2ym - vb3y*vb2xm;
		rasq = ax*ax + ay*ay + az*az;
		rbsq = bx*bx + by*by + bz*bz;
		rgsq = vb2xm*vb2xm + vb2ym*vb2ym + vb2zm*vb2zm;
		rg = sqrt(rgsq);
		rginv = ra2inv = rb2inv = 0.0;
		if (rg > 0) rginv = 1.0/rg;
		if (rasq > 0) ra2inv = 1.0/rasq;
		if (rbsq > 0) rb2inv = 1.0/rbsq;
		rabinv = sqrt(ra2inv*rb2inv);
		c = (ax*bx + ay*by + az*bz)*rabinv;
		s = rg*rabinv*(ax*vb3x + ay*vb3y + az*vb3z);

		df = 0.0;
		int J= dtypes[type].multi;

		if(J>0){
			p_=1.0;
			ddf1_=df1_=0.0;
			for (int loop=0; loop<dtypes[type].coeff2; loop++){        
				ddf1_ = p_*c - df1_*s;
				df1_ = p_*s + df1_*c;
				p_ = ddf1_;
			}
			p_ = p_*dtypes[type].coeff4 + df1_*dtypes[type].coeff5;
			df1_ = df1_*dtypes[type].coeff4 - ddf1_*dtypes[type].coeff5;
			df1_ *= -dtypes[type].coeff2;
			p_ += 1.0;
            if(dtypes[type].coeff2==0){
                p_=1.0+dtypes[type].coeff4;
                df1_=0.0;
                cout<<"OK"<<endl;
            }
			df += (-dtypes[type].coeff1 * df1_);
		}
		if(J>1){
			p_=1.0;
			ddf1_=df1_=0.0;
			for (int loop=0; loop<dtypes[type].coeff7; loop++){        
				ddf1_ = p_*c - df1_*s;
				df1_ = p_*s + df1_*c;
				p_ = ddf1_;
			}
			p_ = p_*dtypes[type].coeff9 + df1_*dtypes[type].coeff10;
			df1_ = df1_*dtypes[type].coeff9 - ddf1_*dtypes[type].coeff10;
			df1_ *= -dtypes[type].coeff7;
			p_ += 1.0;
            if(dtypes[type].coeff7==0){
                p_=1.0+dtypes[type].coeff4;
                df1_=0.0;
            }
			df += (-dtypes[type].coeff6 * df1_);
		}
		if(J>2){
            p_=1.0;
            ddf1_=df1_=0.0;
			for (int loop=0; loop<dtypes[type].coeff12; loop++){        
				ddf1_ = p_*c - df1_*s;
				df1_ = p_*s + df1_*c;
				p_ = ddf1_;
			}
			p_ = p_*dtypes[type].coeff14 + df1_*dtypes[type].coeff15;
			df1_ = df1_*dtypes[type].coeff14 - ddf1_*dtypes[type].coeff15;
			df1_ *= -dtypes[type].coeff12;
			p_ += 1.0;
            if(dtypes[type].coeff12==0){
                p_=1.0+dtypes[type].coeff4;
                df1_=0.0;
            }
			df += (-dtypes[type].coeff11 * df1_);
		}
       // cout<<df<<endl;
		fg = vb1x*vb2xm + vb1y*vb2ym + vb1z*vb2zm;
		hg = vb3x*vb2xm + vb3y*vb2ym + vb3z*vb2zm;
		fga = fg*ra2inv*rginv;
		hgb = hg*rb2inv*rginv;
		gaa = -ra2inv*rg;
		gbb = rb2inv*rg;
		dtfx = gaa*ax;
		dtfy = gaa*ay;
		dtfz = gaa*az;
		dtgx = fga*ax - hgb*bx;
		dtgy = fga*ay - hgb*by;
		dtgz = fga*az - hgb*bz;
		dthx = gbb*bx;
		dthy = gbb*by;
		dthz = gbb*bz;
		sx2 = df*dtgx;
		sy2 = df*dtgy;
		sz2 = df*dtgz;
		ff1[0] = df*dtfx;
		ff1[1] = df*dtfy;
		ff1[2] = df*dtfz;
		ff2[0] = sx2 - ff1[0];
		ff2[1] = sy2 - ff1[1];
		ff2[2] = sz2 - ff1[2];
		ff4[0] = df*dthx;
		ff4[1] = df*dthy;
		ff4[2] = df*dthz;
		ff3[0] = -sx2 - ff4[0];
		ff3[1] = -sy2 - ff4[1];
		ff3[2] = -sz2 - ff4[2];

		ions[i].fx += ff1[0];
		ions[i].fy += ff1[1];
		ions[i].fz += ff1[2];
		ions[j].fx += ff2[0];
		ions[j].fy += ff2[1];
		ions[j].fz += ff2[2];
		ions[k].fx += ff3[0];
		ions[k].fy += ff3[1];
		ions[k].fz += ff3[2];
		ions[l].fx += ff4[0];
		ions[l].fy += ff4[1];
		ions[l].fz += ff4[2];
	
	}
}


