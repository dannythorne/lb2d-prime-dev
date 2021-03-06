//##############################################################################
//
// Copyright (C), 2005, Michael Sukop and Danny Thorne
//
// collide.c
//

#if POROUS_MEDIA
void collide( lattice_ptr lattice)
{
  double *f;

  double omega;

  int    *bc_type;

  int    n, a;

  int    subs;

  double ns;

  double *ftemp;

  int    i,  j;
  int    ip, jp,
         in, jn;
  int    ni = lattice->param.LX,
         nj = lattice->param.LY;

#if SAY_HI
  printf("collide() -- Hi!\n");
#endif /* SAY_HI */

 for( subs=0; subs<NUM_FLUID_COMPONENTS; subs++)
 {

  //dump_pdf( lattice, 998);

  f = lattice->pdf[subs][0].f;

  bc_type = &( lattice->bc[subs][0].bc_type);

  for( n=0; n<lattice->NumNodes; n++, f+=18, bc_type++)
  {
    if( !( *bc_type & BC_SOLID_NODE))
    {
   // if( *bc_type == 0)
   // {
        // C O L L I D E

        for( a=0; a<=8; a++, f++)
        {
          // f = f - (1/tau[subs])( ftemp - feq)
//printf("collide() -- (Before): n = %2d, a = %d, "
//       "*f = %10.7f, *(f+9) = %10.7f, *(f-9) = %10.7f\n",
//        n, a, *(f), *(f+9), *(f-9));


#if 1
          *f = *(f+9) - ( ( *(f+9) / lattice->param.tau[subs] )
                        - ( *(f-9) / lattice->param.tau[subs] ) );
#else
          *f = *(f+9) - ( ( *(f+9) )
                        - ( *(f-9) ) ) / lattice->param.tau[subs];
#endif

#if 0//PERTURBATIONS
          //if( n%lattice->NumNodes == n/lattice->NumNodes && a==5)
          //if( n/lattice->NumNodes == lattice->param.LY/2 && a==1)
          if( a==1)
          {
            //*f+=.0001*(n%lattice->NumNodes)*( rand()/(double)RAND_MAX - .5);
            *f+=.000001*(n%lattice->NumNodes)*( rand()/(double)RAND_MAX);
          }
#endif /* PERTURBATIONS */

#if PUKE_NEGATIVE_DENSITIES
          if( *f < 0.)
          {
            printf("\n");
            printf(
              "collide() -- Node %d (%d,%d), subs %d, "
              "has negative density %20.17f "
              "in direction %d "
              "at timestep %d. Exiting!\n",
              n, n%lattice->param.LX,
                 n/lattice->param.LX,
                 subs,
                 *f, a,
                 lattice->time             );
            printf("\n");
            process_exit(1);
          }
#endif /* PUKE_NEGATIVE_DENSITIES */

//printf("collide() -- (After ): n = %2d, a = %d, "
//       "*f = %10.7f, *(f+9) = %10.7f, *(f-9) = %10.7f\n",
//        n, a, *(f-1), *(f-1+9), *(f-1-9));

        } /* for( a=0; a<=8; a++) */
   // }
   // else
   // {
//printf("collide() -- Skipping bc %d at n = %d\n", *bc_type, n);
   //   *f++ = *( f + 9);
   //   *f++ = *( f + 9);
   //   *f++ = *( f + 9);
   //   *f++ = *( f + 9);
   //   *f++ = *( f + 9);
   //   *f++ = *( f + 9);
   //   *f++ = *( f + 9);
   //   *f++ = *( f + 9);
   //   *f++ = *( f + 9);
   // }

    } /* if( !( *bc_type++ & BC_SOLID_NODE)) */

    else // *bc_type++ & BC_SOLID_NODE
    {
      // B O U N C E B A C K

      f++; // Skip rest particle.

      *f++ = *( f + 9 + 2); //f++; // f[1] = ftemp[3]
      *f++ = *( f + 9 + 2); //f++; // f[2] = ftemp[4]
      *f++ = *( f + 9 - 2); //f++; // f[3] = ftemp[1]
      *f++ = *( f + 9 - 2); //f++; // f[4] = ftemp[2]
      *f++ = *( f + 9 + 2); //f++; // f[5] = ftemp[7]
      *f++ = *( f + 9 + 2); //f++; // f[6] = ftemp[8]
      *f++ = *( f + 9 - 2); //f++; // f[7] = ftemp[5]
      *f++ = *( f + 9 - 2); //f++; // f[8] = ftemp[6]

//printf("collide() -- Bncback: n = %2d\n", n);

    } /* if( !( *bc_type++ & BC_SOLID_NODE)) else */

  } /* for( n=0; n<lattice_NumNodes; n++) */

  if( subs==0)
  {
    // Compute the solid density term for fluid component.
    ftemp   = lattice->pdf[subs][0].ftemp;
    bc_type = &( lattice->bc[subs][0].bc_type);
    for( n=0; n<lattice->NumNodes; n++, ftemp+=18, bc_type++)
    {
      i = n%ni;
      j = n/ni;

      jp = ( j<nj-1)?( j+1):( 0   );
      jn = ( j>0   )?( j-1):( nj-1);

      ip = ( i<ni-1)?( i+1):( 0   );
      in = ( i>0   )?( i-1):( ni-1);

      if( !( *bc_type & BC_SOLID_NODE))
      {
        if( lattice->param.ns >= 0.)
        {
          ns = lattice->param.ns;

  /* 0 */ ftemp++;

  /* 1 */ *ftemp++ = ns*( lattice->pdf[subs][ j *ni + ip].f[3]
                        - lattice->pdf[subs][ j *ni + i ].f[1]);
  /* 2 */ *ftemp++ = ns*( lattice->pdf[subs][ jp*ni + i ].f[4]
                        - lattice->pdf[subs][ j *ni + i ].f[2]);
  /* 3 */ *ftemp++ = ns*( lattice->pdf[subs][ j *ni + in].f[1]
                        - lattice->pdf[subs][ j *ni + i ].f[3]);
  /* 4 */ *ftemp++ = ns*( lattice->pdf[subs][ jn*ni + i ].f[2]
                        - lattice->pdf[subs][ j *ni + i ].f[4]);
  /* 5 */ *ftemp++ = ns*( lattice->pdf[subs][ jp*ni + ip].f[7]
                        - lattice->pdf[subs][ j *ni + i ].f[5]);
  /* 6 */ *ftemp++ = ns*( lattice->pdf[subs][ jp*ni + in].f[8]
                        - lattice->pdf[subs][ j *ni + i ].f[6]);
  /* 7 */ *ftemp++ = ns*( lattice->pdf[subs][ jn*ni + in].f[5]
                        - lattice->pdf[subs][ j *ni + i ].f[7]);
  /* 8 */ *ftemp++ = ns*( lattice->pdf[subs][ jn*ni + ip].f[6]
                        - lattice->pdf[subs][ j *ni + i ].f[8]);
        }
        else
        {
          // TODO: Variable solid density.
        }
      } /* if( !( *bc_type++ & BC_SOLID_NODE)) */
      else
      {
        ftemp+=9;
      }
    } /* for( n=0; n<lattice_NumNodes; n++) */

    f = lattice->pdf[subs][0].f;
    bc_type = &( lattice->bc[subs][0].bc_type);
    for( n=0; n<lattice->NumNodes; n++, f+=18, bc_type++)
    {
      if( !( *bc_type & BC_SOLID_NODE))
      {
        f++;
        for( a=1; a<9; a++, f++)
        {
          *f += *(f+9);

        } /* for( a=1; a<9; a++) */
      }
      else
      {
        f+=9;
      }
    } /* for( n=0; n<lattice->NumNodes; n++, f+=18) */

  } /* if( subs==0) */

  //dump_pdf( lattice, 999);

 } /* for( subs=0; subs<NUM_FLUID_COMPONENTS; subs++) */

#if SAY_HI
  printf("collide() -- Bye!\n");
#endif /* SAY_HI */

} /* void collide( lattice_ptr lattice) */
#else /* !( POROUS_MEDIA) */
void collide( lattice_ptr lattice)
{
  double *feq;
  double *f;
  double *ftemp;
#if ZHANG_AND_CHEN_ENERGY_TRANSPORT
  double *force;
#endif /* ZHANG_AND_CHEN_ENERGY_TRANSPORT */

  double omega;

  int    bc_type;

  int    n, a;

  int    subs;

#if SAY_HI
  printf("collide() -- Hi!\n");
#endif /* SAY_HI */

 for( subs=0; subs<NUM_FLUID_COMPONENTS; subs++)
 {

  for( n=0; n<lattice->NumNodes; n++)
  {
    feq   = lattice->pdf[subs][n].feq;
    f     = lattice->pdf[subs][n].f;
    ftemp = lattice->pdf[subs][n].ftemp;
    bc_type = lattice->bc[subs][n].bc_type;
#if ZHANG_AND_CHEN_ENERGY_TRANSPORT
    force = lattice->force[subs][n].force;
#endif /* ZHANG_AND_CHEN_ENERGY_TRANSPORT */

    if( !( bc_type & BC_SOLID_NODE))
    {
        // C O L L I D E

        // f = ftemp - (1/tau[subs])( ftemp - feq)
        for( a=0; a<=8; a++)
        {
#if 1
          f[a] = ftemp[a] - ( ( f[a]   / lattice->param.tau[subs] )
                            - ( feq[a] / lattice->param.tau[subs] ) );
#else
          f[a] = ftemp[a] - ( ( ftemp[a] )
                            - ( feq[a]   ) ) / lattice->param.tau[subs];
#endif
        } /* for( a=0; a<=8; a++) */

#if ZHANG_AND_CHEN_ENERGY_TRANSPORT
        //
        // Add the body force term, equation (8),
        //
        //   f_i = f_i + \Delta f_i
        //
        //       = f_i + \frac{w_i}{T_0} c_i \dot F
        //
        // Assuming the weights, w_i, are the ones from compute_feq.
        //
        // Zhang & Chen state T_0 to be 1/3 for D3Q19.  The same in D2Q9.
        //

        f[1] += feq[1]*( 3.*2.*force[0]);
        f[2] += feq[2]*( 3.*2.*force[1]);
        f[3] += feq[3]*( 3.*2.*force[0]);
        f[4] += feq[4]*( 3.*2.*force[1]);

        f[5] += feq[5]*( 3.*(  force[0] + force[1]));
        f[6] += feq[6]*( 3.*(  force[0] + force[1]));
        f[7] += feq[7]*( 3.*(  force[0] + force[1]));
        f[8] += feq[8]*( 3.*(  force[0] + force[1]));

#endif /* ZHANG_AND_CHEN_ENERGY_TRANSPORT */

#if PUKE_NEGATIVE_DENSITIES
        for( a=0; a<=8; a++)
        {
          if( *f < 0.)
          {
            printf("\n");
            printf(
              "collide() -- Node %d (%d,%d), subs %d, "
              "has negative density %20.17f "
              "in direction %d "
              "at timestep %d. Exiting!\n",
              n, n%lattice->param.LX,
                 n/lattice->param.LX,
                 subs,
                 f[a], a,
                 lattice->time             );
            printf("\n");
            process_exit(1);
          }
        } /* for( a=0; a<=8; a++) */
#endif /* PUKE_NEGATIVE_DENSITIES */

    } /* if( !( bc_type & BC_SOLID_NODE)) */

    else // bc_type & BC_SOLID_NODE
    {
      // B O U N C E B A C K

      if(   lattice->param.bc_slip_north
         && n >= lattice->NumNodes - lattice->param.LX)
      {
        // Slip condition on north boundary.
        f[1] = ftemp[1];
        f[2] = ftemp[4];
        f[3] = ftemp[3];
        f[4] = ftemp[2];
        f[5] = ftemp[8];
        f[6] = ftemp[7];
        f[7] = ftemp[6];
        f[8] = ftemp[5];
      }
      else
      {
        // Usual non-slip bounce-back condition.
        f[1] = ftemp[3];
        f[2] = ftemp[4];
        f[3] = ftemp[1];
        f[4] = ftemp[2];
        f[5] = ftemp[7];
        f[6] = ftemp[8];
        f[7] = ftemp[5];
        f[8] = ftemp[6];
      }

    } /* if( !( bc_type & BC_SOLID_NODE)) else */

  } /* for( n=0; n<lattice_NumNodes; n++) */

 } /* for( subs=0; subs<NUM_FLUID_COMPONENTS; subs++) */

#if SAY_HI
  printf("collide() -- Bye!\n");
#endif /* SAY_HI */

} /* void collide( lattice_ptr lattice) */
#endif /* POROUS_MEDIA */
