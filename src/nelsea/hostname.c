/* static char *SccsId = "@(#)hostname.c 3.52 (TU-Delft) 03/13/02"; */
/**********************************************************

Name/Version      : nelsea/3.52

Language          : C
Operating system  : UNIX SYSTEM V
Host machine      : any

Author(s)         : Patrick Groeneveld and Paul Stravers
Creation date     : april 1992
Modified by       : 
Modification date : April 15, 1992


        Delft University of Technology
        Department of Electrical Engineering
        Mekelweg 4 - P.O.Box 5031
        2600 GA DELFT
        The Netherlands

        Phone : 015 - 786240

        COPYRIGHT (C) 1992 , All rights reserved
**********************************************************/
/* hostname.c, copied to a large extend from nelsis R4 dmi */

/*  Functions which provide conversion of paths from
 *  local to remote and vice versa, using NFS-mount tables,
 *  Domain-net, NS9000 etc.
 */

/* Patrick */
#include "prototypes.h"
#include "def.h"
#include "nelsis.h"
#ifdef CPLUSPLUSHACK
#include "sysdep.h"
#endif

#define SYSV

#ifdef __hp9000s300
#define HPCLUSTER
#endif 

/*
#include "dmerror.h"
#include "rcerror.h"
#include "unixerr.h"
 

#include "dmdebug.h"
#include "dmdef.h"
#include "dmi.h"
 */

/* * * * * * * * * 
 *
 * Nelsis error function
 *
 *   dmError(char *s );
 */

/* extern int chdir(char *); */

#ifndef DM_MAXPATHLEN
#define DM_MAXPATHLEN 256
#endif
#define NM_SIZE 256
#define RcErrorClass 0
#define RCEBADARG 0
#ifndef DMSYSERROR
#   define DMSYSERROR 0
#endif
#define DMNOTFATAL 0
#define RCEGETHOSTNAME 0
#define RCEFOPENFAIL 0
#define RCEMOUNTTABERR 0
#define RCEDOMAINCONV 0
#define UnixErrorClass 0
#define RCEPATHCONV 0
#define RCEPATHNOEXIST 0
#define RCENOCLUSTERROOT 0
#define errno 0
#define ASSERT(a)


#ifdef apollo
#include <sys/types.h>
#include <mtab.h>
#define  MOUNTTAB	MTAB
#define  MNTENTARG	MTAB_FILE
#else /* not apollo */

#if defined( hp9000s300 ) || defined( HPCLUSTER )	/* hp cluster info */
/* typedef unsigned char u_char; */
/* typedef unsigned short int cnode_t; */
#include <cluster.h>
#endif /* defined( hp9000s300 ) || defined( HPCLUSTER ) */

/* typedef unsigned short int cnode_t; */
#define _INCLUDE_HPUX_SOURCE

#ifdef __svr4__                 /* e.g. Solaris */
#include <sys/mnttab.h>
#define MOUNTTAB MNTTAB
#define setmntent fopen
#define endmntent fclose
#include <sys/systeminfo.h>
#define gethostname(buf,siz) sysinfo(SI_HOSTNAME,buf,siz)
#else
#include <mntent.h>
#endif
#define  MNTENTARG	(fp)

#ifndef MNTTYPE_NFS /* for instance on linux-0.99 it's missing from mntent.h */
#define MNTTYPE_NFS "nfs"
#endif

/* the include file <mntent.h> #define's the name of the mount table file
 * either as MNT_MNTTAB or as MOUNTED, dependend on whether we're on SYS-V or
 * on a BSD system...
 */
#if defined(MNT_MNTTAB)
#   define MOUNTTAB MNT_MNTTAB
#elif defined(MOUNTED)
#   define MOUNTTAB MOUNTED
#endif

#endif /* apollo */

#include <sys/types.h>
#include <sys/stat.h>

/* #if defined( hp9000s300 ) || defined( HPCLUSTER )
#include <cluster.h>
#endif 
*/

/* Extern Declarations */
/* extern int	gethostname (char *, unsigned int);
extern char	*cs ();
 */
#ifdef CPLUSPLUSHACK
#ifndef gethostname
extern int	gethostname (char *, unsigned int);
#endif
#endif 

#if defined(sun) && !defined(__svr4__)
#include <sys/param.h>
extern char	*getwd ();
#else /* !SunOS */
#include <signal.h>
extern char	*getcwd ();
#endif /* sun */

/* Local Declarations */
static int matchPaths (char * path, char * mnt_dir);
static int existPath (char * path);
static char * _dmGetDomainHost (char * path);
static int noSignalError(int, int, int, int, int, char *, char *);


/* static void noSignalError (int n1, int n2, int n3, int n4, int n5, char * err, char * tt); */

/*
 * hack routine, firt we used only stringsaves
 */
static char * cs_hack( char * str)
{
/* char * p; */

return(cs(str));
/*
p = (char *)malloc(strlen(str)+1);
if(!p) 
   error(FATAL_ERROR,"malloc_stringsave");
strcpy(p, str);
return(p); */
}


/*  Remote To Local Path conversion:
 *  Given a hostname and absolute path (on the 
 *  disk with name == hostname) 
 *  an absolute path from the local host 
 *  (== host where the tool is executed on) 
 *  is composed and (StrSaved) returned.
 */
char * RemToLocPath (char * hostname, char * rem_path)
{
#ifdef apollo
    int			fp;
#else
    FILE		*fp;
#endif
#ifdef __svr4__
    struct mnttab       mountinfo;
    struct mnttab	*mntinfo = &mountinfo;
#else
    struct mntent	*mntinfo;
#endif
    char		*fs_path = NULL;		/* file-system path */
    char		*fs_hostname = NULL;	/* file-system hostname */
    char		*loc_path = NULL;		/* local path */
    char		*domain_host = NULL;
    char		*automount_dir = NULL;	/* sun's automount directory */
    char		tmp[DM_MAXPATHLEN];
    char		hn[NM_SIZE];		/* temp. hostname */
    char		new_path[DM_MAXPATHLEN];	/* temp. local_path */
    int			score = 0;		/* match score */
    int    		max_score = 0;		/* maximal match score */
    int			NotNFS_flag = FALSE;	/* path not mounted by NFS */

    /* Init of certain vars */
    *new_path = '\0';

    /* Arg check: no NULL args allowed */
    /* and absolute path required      */
    if (hostname == NULL || rem_path == NULL || *rem_path != '/') {
	(void) noSignalError (RcErrorClass, RCEBADARG, 
		DMSYSERROR, DMNOTFATAL, 0, "dmRemToLocPath", NULL);
	return (NULL);
    }

    /* Step 1.
     *    If specified hostname == local hostname:
     * 	  Return rem_path.
     */
    if (gethostname (hn, NM_SIZE) < 0) {
	(void) noSignalError (RcErrorClass, RCEGETHOSTNAME, 
		DMSYSERROR, DMNOTFATAL, 0, "dmRemToLocPath", NULL);
	return (NULL);
    }

    if (strcmp (hostname, hn) == 0) {

#if defined( hp9000s300 ) || defined( HPCLUSTER )	/* hp cluster */
	/* This test is not neccesary but signals some incorrect projID's */
	/* hostname == cluster root ? */
	(void) _dmTestIfClusterRoot (hostname);
#endif /* defined( hp9000s300 ) || defined( HPCLUSTER ) */

	(void) strcpy (new_path, rem_path);

	goto exit_point;
    }

#ifndef hp9000s500	/* no NFS running */
    /* Step 2.
     *    Get NFS mount info and lookup for automount directory
     */
#ifdef apollo
    if ((fp = setmntent MTAB_FILE) <= 0)
#else
    if ((fp = setmntent (MOUNTTAB, "r")) == NULL)
#endif /* apollo */
    {
	(void) noSignalError (RcErrorClass, RCEFOPENFAIL, 
		DMSYSERROR, DMNOTFATAL, 0, "NFS-mounttable", NULL);
	return (NULL);
    }

#ifdef __svr4__
    for (score = max_score = 0; getmntent(fp,mntinfo) == 0;)
#else
    for (mntinfo = getmntent MNTENTARG, score = max_score = 0;
 	 mntinfo != NULL; mntinfo = getmntent MNTENTARG)
#endif
    {

	/* consider only NFS mounts */
#ifdef __svr4__
	if (strcmp (mntinfo->mnt_fstype, MNTTYPE_NFS) != 0)
#else
	if (strcmp (mntinfo->mnt_type, MNTTYPE_NFS) != 0)
#endif
        {
	    continue;
	}
#ifdef __svr4__
        if ((fs_hostname = getHostNamePart (mntinfo->mnt_special)) == NULL)
#else
	if ((fs_hostname = getHostNamePart (mntinfo->mnt_fsname)) == NULL)
#endif
        {
	    (void) noSignalError (RcErrorClass, RCEMOUNTTABERR, 
		    DMSYSERROR, DMNOTFATAL, 0, "fs_hostname:hnpart", NULL);
	    continue;
	}
	if (strcmp (fs_hostname, hostname) == 0) {
#ifdef __svr4__
	    if ((fs_path = getPathPart (mntinfo->mnt_special)) == NULL)
#else
	    if ((fs_path = getPathPart (mntinfo->mnt_fsname)) == NULL)
#endif
            {
		(void) noSignalError (RcErrorClass, RCEMOUNTTABERR, 
			DMSYSERROR, DMNOTFATAL, 0, 
			"fs_hostname:pathpart", NULL);
	        continue;
	    }
	    score = matchPaths (rem_path, fs_path);
#ifdef DM_DEBUG
            IFDEBUG {
               fprintf (stderr,"RemToLoc:hostnm: %s, fs_path: %s,score: %d\n",
                        fs_hostname, fs_path, score);
            }
#endif /* DM_DEBUG */
	    if (score > max_score) {
               max_score = score;
#ifdef __svr4__
               (void) strcpy (tmp, mntinfo->mnt_mountp);
#else
               (void) strcpy (tmp, mntinfo->mnt_dir);
#endif
#ifdef DM_DEBUG
               IFDEBUG {
                  fprintf (stderr,"RemToLoc: new mnt_dir: %s\n", tmp);
               }
#endif /* DM_DEBUG */
	    }
	}

#ifdef sun
	/* look for automount process */
	if (strncmp (fs_hostname, "(pid", 4) == 0) {
#ifdef __svr4__
           automount_dir = cs_hack (mntinfo->mnt_mountp);
#else
           automount_dir = cs_hack (mntinfo->mnt_dir);
#endif
	}
#endif /* sun */

    }

    (void) endmntent MNTENTARG;

#endif /* hp9000s500 */

    switch (max_score) {
	case 0:
	    /*  Hostname and rem_path not mounted via NFS
	     */
	    NotNFS_flag = TRUE;
	    break;
        case 1:
	    /*
	     * Example: dutedid:/ mounted on /dutedid.
	     * The matches are:
	     *    specified hostname == dutedid, mounted fs == '/'
	     * If specified rem_path is only "/" then skip it, else 
	     * glue it to the mounted directory (/dutedid/.../..)
	     * Remark: score == 0 means that '/''ses are involved,
	     *         because only absolute paths can be mounted!
	     */
	    if (strcmp (rem_path, "/") != 0) {
		max_score = 0;
	    }
	    break;
        case 2:
	    /*
	     * Example: dutepiz:// mounted on /apollo.
	     * The matches are:
	     *    specified hostname == dutepiz, mounted fs == '//'
	     * If specified rem_path is only "//" then skip it, else glue it 
	     * with a slash in between to the mounted directory (/apollo/.../..)
	     */
	    if ((*(rem_path + 1) == '/') &&
		(*(rem_path + 2) != '\0')){
		max_score = 1;
	    }
	    break;
	default:
	    break;
    }

    if (NotNFS_flag == FALSE) {
	(void) strcpy (new_path, strcat (tmp, rem_path + max_score));
	goto exit_point; 
    }

#if defined( hp9000s300 ) || defined( HPCLUSTER )	/* hp cluster */
    /* Step 3.
     *    if hostname == member of this cluster ( && cluster root:) ready
     *    else: continue with trial
     *    (possible warning generated by dmTestIfClusterRoot)
     */
    if (_dmTestIfClusterRoot (hostname) == TRUE) {
	(void) strcpy (new_path, rem_path);
	goto exit_point;
    }
#endif /* defined( hp9000s300 ) || defined( HPCLUSTER ) */

#ifdef apollo
    /* Step 4.
     *    '//hostname/loc_path' is possible.
     */

    if ((*rem_path == '/') && (*(rem_path + 1) == '/')) {
	/* is already an abs. apollo path */
	(void) strcpy (new_path, rem_path);
    }
    else {
	/* make an absolute apollo path */
	(void) sprintf (new_path, "//%s%s", hostname, rem_path);
    }
    goto exit_point;
#endif /* apollo */

#ifdef sun
    /* Step 5.
     *    Try to use the automounter.
     */

    if (automount_dir != NULL) {
	(void) sprintf (new_path, "%s/%s%s",
			    automount_dir, hostname, rem_path);
    }
    goto exit_point;
#endif

#ifdef hpux
    /* Step 6.
     *    Try to use the NS9000.
     */

    (void) sprintf (new_path, "/net/%s%s", hostname, rem_path);
    goto exit_point;
#endif /* hpux */


exit_point:

    /* Step 7.
     *    Check if it wasn't possible to create 
     *    a local path.
     *    Test the obtained path for existence.
     */
    if ((*new_path != '\0') && (existPath (new_path) == 0)) {

	/* Additional test:
	 * Test whether the new obtained path is a domain-path 
	 * (starting with '//')
	 * and if yes, whether hostname equals the domain host
	 */
	if ((domain_host = _dmGetDomainHost (new_path)) != NULL) {
	    if (strcmp (hostname, domain_host) != 0) {
		(void) noSignalError (RcErrorClass, RCEDOMAINCONV, 
			DMSYSERROR, DMNOTFATAL, 0, NULL, NULL);
	    }
	}
	return (cs_hack (new_path));
    }
    else {
	return (NULL); 	  /* error signaled by existPath */
    }
}

/* temp test... */
char * LL(char *hostname, char * loc_path)
{
char HH[256], LL[256], * ret;

if(hostname != NULL)
   strcpy(HH, hostname);
if(loc_path != NULL)
   strcpy(LL, loc_path);

ret = LocToRemPath(& hostname, & loc_path);

printf("hostname = '%s', loc_path = '%s'\n", hostname, loc_path);
return(ret);
}

/*
 *  Local To Remote Path:
 *  Transforms the local path (abs. or rel.) to the corresponding hostname
 *  and abs. path from the root of the disk with name 'hostname'.
 *  If loc_path is a relative path, it will be changed to an abs. path.
 *  (and returned)
 *  If the (prefix of the) local path has not been mounted locally,
 *  than the specified loc_path is returned, hostname points to
 *  the local host and dmerrno is set to DME_NOMOUNT.
 *  Remark: returns are StrSaved.
 */
char * LocToRemPath (char **hostname, char ** loc_path)
{
#ifdef apollo
    int		fp;
#else
    FILE	*fp;
#endif /* apollo */
#ifdef __svr4__
    struct mnttab mountinfo;
    struct mnttab *mntinfo = &mountinfo;
#else
    struct mntent *mntinfo;
#endif
    char	*tmp = NULL;
    char	*rem_path = NULL;
    char	*domain_host = NULL;
    /* Paul: tmphost and tmppath are local char arrays, or we dump core ... */
    char        tmphost[DM_MAXPATHLEN];
    char        *tmphost1;
    char	tmppath[DM_MAXPATHLEN];
    char        *tmppath1;
    char	hn[NM_SIZE];
#if defined(sun) && !defined(__svr4__)
    char	new_path[MAXPATHLEN];
    char	cwd[MAXPATHLEN];
#else /* !SunOS */
    char	new_path[DM_MAXPATHLEN];
    char	cwd[DM_MAXPATHLEN];     /* to avoid shitty mem problems PATRICK */
/*     char	*cwd = NULL;  OLD */ 
/*    void	(*osighand)(SIG_PF_TYPE); */
#endif /* sun */
    int		score = 0;
    int		max_score = 0;
    int error = FALSE;

    /* Arg check, poss. test local path */
    if (*loc_path == NULL) {
	(void) noSignalError (RcErrorClass, RCEBADARG, 
		DMSYSERROR, DMNOTFATAL, 0, "dmLocToRemPath", NULL);
	return (NULL);
    }

    /* Step 1.
     *     Convert loc_path to an abs. path 
     */

    /* NOTE: SIGCLD handlers may cause problems in the operation
     * of getcwd (due to the death of the popened 'pwd' process)
     * therefore this signal must be ignored (PB).
     */

#ifdef SYSV
/*    osighand = (*)(SIG_PF_TYPE) signal(SIGCLD, (SIG_PF_TYPE) SIG_IGN); */
#else /* SYSV */
#ifndef sun
    osighand = (*)(SIG_PF_TYPE) signal(SIGCHLD, (SIG_PF_TYPE) SIG_IGN);
#endif
#endif /* SYSV */

     error = FALSE;
#if defined(sun) && !defined(__svr4__)
     if (getwd (cwd) == NULL)
#else
     if (getcwd (cwd, DM_MAXPATHLEN) == NULL)
#endif
     {
     fprintf(stderr,"ERROR 1\n");
     error = TRUE;
     }
     if(chdir (*loc_path) != 0)
     {
     fprintf(stderr,"ERROR 2\n");
     error = TRUE;
     }
#if defined(sun) && !defined(__svr4__)
     if(getwd (new_path) == NULL)
#else
     if(getcwd (new_path, DM_MAXPATHLEN) == NULL)
#endif
     {
     fprintf(stderr,"ERROR 3: I am not allowed to access '%s'\n", *loc_path);
     error = TRUE;
     }
     if(chdir (cwd) != 0)
     {
     fprintf(stderr,"ERROR 4\n");
     error = TRUE;
     }

    if (error == FALSE)
      {

#ifndef sun
#ifdef SYSV
/*	    (void) signal(SIGCLD, osighand); */
#else /* SYSV */
	    (void) signal(SIGCHLD, osighand);
#endif /* SYSV */
#endif /* !sun */
	    *loc_path = cs_hack (new_path);
    }
    else {
#ifndef sun
#ifdef SYSV
/*	(void) signal(SIGCLD, osighand); */
#else /* SYSV */
	(void) signal(SIGCHLD, osighand);
#endif /* SYSV */
#endif /* !sun */
	(void) noSignalError (UnixErrorClass, errno, 
		DMSYSERROR, DMNOTFATAL, 0, NULL, NULL);
	(void) noSignalError (RcErrorClass, RCEPATHCONV, 
		DMSYSERROR, DMNOTFATAL, 0, *loc_path, NULL);
	return (NULL);
    }

#ifndef hp9000s500	/* no NFS running on them */
    /* Step 2.
     *     Try to find a NFS-mounttable slot
     */
#ifdef apollo
    if ((fp = setmntent MTAB_FILE) <= 0)
#else
    if ((fp = setmntent (MOUNTTAB, "r")) == NULL)
#endif /* apollo */
    {
	(void) noSignalError (RcErrorClass, RCEFOPENFAIL, 
		DMSYSERROR, DMNOTFATAL, 0, "NFS-mounttable", NULL);
	return (NULL);
    }

#ifdef __svr4__
    for (score = max_score = 0; getmntent(fp,mntinfo) == 0;)
#else
    for (mntinfo = getmntent MNTENTARG, score = max_score = 0;
         mntinfo != NULL; mntinfo = getmntent MNTENTARG)
#endif
    {

	/* consider only NFS mounts */
#ifdef __svr4__
	if (strcmp (mntinfo->mnt_fstype, MNTTYPE_NFS) != 0)
#else
	if (strcmp (mntinfo->mnt_type, MNTTYPE_NFS) != 0)
#endif
        {
	    continue;
	}
#ifdef __svr4__
	score = matchPaths (*loc_path, mntinfo->mnt_mountp);
#else
	score = matchPaths (*loc_path, mntinfo->mnt_dir);
#endif
	if (score > max_score) {
#ifdef __svr4__
	    if ((tmphost1 = getHostNamePart (mntinfo->mnt_special))==NULL)
#else
	    if ((tmphost1 = getHostNamePart (mntinfo->mnt_fsname))==NULL)
#endif
            {
		(void) noSignalError (RcErrorClass, RCEMOUNTTABERR, 
			DMSYSERROR, DMNOTFATAL, 0, "fs_hostname:hnpart", NULL);
		continue;
	    }
            strcpy(tmphost,tmphost1); /* tmphost1 is canonic but we want
                                       * to modify tmphost later */
#ifdef __svr4__
	    if ((tmppath1 = getPathPart (mntinfo->mnt_special)) == NULL)
#else
	    if ((tmppath1 = getPathPart (mntinfo->mnt_fsname)) == NULL)
#endif
            {
		(void) noSignalError (RcErrorClass, RCEMOUNTTABERR, 
			DMSYSERROR, DMNOTFATAL, 0, 
			"fs_hostname:pathpart", NULL);
		continue;
	    }
            strcpy(tmppath,tmppath1); /* tmppath1 is canonic, cannot modify */
               
	    /* set these values only if both parts were retrieved ok */
	    max_score = score;
	    *hostname = tmphost;
	    tmp = tmppath;
#ifdef DM_DEBUG
            IFDEBUG {
               fprintf (stderr, "LocToRem:sc %d,hostname %s, mnt_dir %s\n",
                        score, *hostname, tmp);
            }
#endif /* DM_DEBUG */

	}

#ifdef DM_DEBUG
        IFDEBUG {
#ifdef __svr4__
           fprintf (stderr, "LocToRem: sc %d,fs_name %s\n",
                    score, mntinfo->mnt_special);
#else
           fprintf (stderr, "LocToRem: sc %d,fs_name %s\n",
                    score, mntinfo->mnt_fsname);
#endif
	}
#endif /* DM_DEBUG */
    }

    (void) endmntent MNTENTARG;

    if (max_score > 0) {
	/* 
	 *  Test whether loc_path and mounted directory match
	 *  in full. If there remains a part of the loc_part,
	 *  and the mounted file-system is only '/' or '//'
	 *  then skip the first slash of that remainder.
	 *  e.g. mount dutente:/ /mnt/dutente
	 *  with *loc_path == /mnt/dutente/tmp/abc
	 */
	if ((*loc_path + max_score) != '\0') {
	    if ((strcmp (tmp, "/") == 0) ||  
		(strcmp (tmp, "//") == 0)) {

		max_score++;	/* skip slash of remainder of loc_path */
	    }
	    /* if there remains part of loc_path and the mount dir
	     * is only '/' or '//' then add a slash
	     * e.g. mount dutedix:/export/root/dutepil /
	     * with *loc_path == /tmp/abc
	     */
	    else if (max_score == 1 ||
		    (max_score == 2 && *loc_path[1] == '/')) {
		max_score--;
	    }
	}

	*hostname = cs_hack (*hostname);
	rem_path = cs_hack (strcat (tmp, *loc_path + max_score));
    }


    /* Step 2. continued
     *     Test whether rem_path is a domain-path (starting with '//'
     *     and if so, make the hostname equals the obtained domain_host.
     */
    if (rem_path != NULL) {
	if((domain_host = _dmGetDomainHost (rem_path)) != NULL) {

	    if (strcmp (*hostname, domain_host) != 0) {
		/* At this point you can try to know whether
		 * the domain_host is really an Apollo.
		 * If not:
		 *	 dmerrno = DME_DOMAIN; 
		 *   dmError ("dmRemToLocPath, Domain convention violation");
		 * In every case:
		 * 	 Change hostname to the obtained domain_host 
		 */
		(void) free (*hostname);
		*hostname = cs_hack (domain_host);
	    }
	}

	return (rem_path);
    }

#endif /* hp9000s500 */


#ifdef hpux
    /* Step 3.
     *    If no rem_path at this point, we go further!
     *    Try to use the NS9000.
     *	  loc_path has form '/net/host/rem_path'
     */
    if (strncmp ("/net/", *loc_path, 5) == 0) {
	int	i;
	char    *h;
        char	net_host [NM_SIZE];

	for (i = 0, h = *loc_path + 5; (*h != '\0') && (*h != '/'); i++, h++) {
	    *(net_host + i) = *h;
	}
	*(net_host + i) = '\0';

	for (i = 0; (*h != '\0'); i++, h++) {
	    *(new_path + i) = *h;
	}
	*(new_path + i) = '\0';

	/* finished ! */
	*hostname = cs_hack (net_host);
	return (cs_hack (new_path)); 
    }
#endif /* hpux */

#if defined( hp9000s300 ) || defined( HPCLUSTER )	/* hp cluster */
  {
    /* Step 4.
     *    Look if 'hn' is a cluster member.
     *    If so, return the name of the cluster root
     */
    struct cct_entry  *cp;
    char	root_host[NM_SIZE];
    int		member_flag = FALSE;
        
    if (gethostname (hn, NM_SIZE) < 0) {
	(void) noSignalError (RcErrorClass, RCEGETHOSTNAME, 
		DMSYSERROR, DMNOTFATAL, 0, "dmLocToRemPath", NULL);
	return (NULL);
    }

    for (cp = getccent (); cp != NULL; cp = getccent ()) {
	if (cp->cnode_type == 'r') {	/* is it the root ? */
	    (void) strcpy (root_host, cp->cnode_name);
	}
	if ((*cp->cnode_name != '\0') && 
	    (strcmp (hn, cp->cnode_name) == 0)) {	/* is it a member ? */
	    member_flag = TRUE;
	}
    }
    (void) endccent ();

    if (member_flag == TRUE) {
	 *hostname = cs_hack (root_host);

	/* finished! return local path */
	return (cs_hack (*loc_path));
    }
  }
#endif /* defined( hp9000s300 ) || defined( HPCLUSTER ) */


    /* Step 5.
     *    The only conclusion which can be drawn at this point:
     *    path is local (the path [or a part of it] isn't mounted or such),
     *    so return local hostname and loc_path.
     *    The local-path has been tested under step 1.
     */
    if (gethostname (hn, NM_SIZE) < 0) {
	(void) noSignalError (RcErrorClass, RCEGETHOSTNAME, 
		DMSYSERROR, DMNOTFATAL, 0, "dmLocToRemPath", NULL);
	return (NULL);
    }

    /* test Domain host */
    if (((domain_host = _dmGetDomainHost (*loc_path)) != NULL) &&
	(strcmp (hn, domain_host) != 0)) {

	*hostname = cs_hack (domain_host);
    }
    else {
	*hostname = cs_hack (hn);
    }

    return (cs_hack (*loc_path));
}


/*
 *  dmMatchPaths qualifies the match of mnt_dir 
 *  (designating the mounted directory or
 *  the fs path prefix) to path.
 */
static int matchPaths (char * path, char * mnt_dir)
{
    int		score;

    /*
     * Constraints: at the end-point + 1 position of equality
     * must be in force:
     * - mnt_dir is at its end
     * - path is at its end or starts with a new (sub)dir {'/'}
     * if not satisfied: match score = 0.
     * There is however one exception: if mnt_dir == "/";
     * in this case the score is set on 1
     */

    /* Arg checks: */
    ASSERT ((path != NULL) && (mnt_dir != NULL));

    if ((path == NULL) || (mnt_dir == NULL)) {
	return (0);
    }

    /* Exception_1: */
    if ((strcmp (mnt_dir, "/") == 0) && (*path == '/')) {
	return (1);
    }

    /* Exception_2: Domain/apollo network root */
    if ((strcmp (mnt_dir, "//") == 0) && (*path == '/') && (*(path+1) == '/')) {
	return (2);
    }

    /* Normal cases: */
    for (score = 0; (*path != '\0') && 
		    (*mnt_dir != '\0') &&
		    (*path == *mnt_dir);   path++, mnt_dir++, score++);

    /* A match of a number of whole segments 
     * (==sub-directories) is required;
     * e.g. mnt_dir '/dutentf/usr' doesnot match with
     * path '/dutentf/usrx'
     */
    if ((*mnt_dir == '\0') && ((*path == '\0') || (*path == '/'))) {
	return (score);
    }
    else {
	return (0);
    }
}


/*
 * dmGetHostNamePart: returns a char pointer 
 * to the 'hostname' part of a 'hostname:path' 
 * string (e.g. as in first column in mounttable).
 */
char * getHostNamePart (char * mnt_fsname)
{
    char	hostname[NM_SIZE];
    char	*hn;

    ASSERT (mnt_fsname);

    if (mnt_fsname == NULL) {
	return (NULL);
    }
    if ((*mnt_fsname == ':') || (*mnt_fsname == '\0')) {
	/* no hostname part, only ':' or '\0' */
	return (NULL);
    }

    hn = hostname;
    for (; (*mnt_fsname != ':') && (*mnt_fsname != '\0');
					*hn++ = *mnt_fsname++); 

    /* ':' required! */
    ASSERT (*mnt_fsname == ':');
    if (*mnt_fsname == ':') {
	*hn = '\0';
	return (cs_hack(hostname));
    }
    else {
	/* no hostname part */
	return (NULL);
    }
}


/*
 *  dmGetPathPart: returns a char pointer to the 'path'
 *  part of a 'hostname:path' string 
 *  (e.g. as in first column in mounttable).
 */
#define MYSTRINGSAVE(ptr, str) \
{ \
printf("in stringsave: ptr = %ld\n", (long) ptr);\
ptr  = (char *)malloc(strlen(str)+1);\
printf("in stringsave: ptr = %ld\n", (long) ptr);\
fflush(stdout);\
if(!ptr) error(FATAL_ERROR,"malloc_stringsave");\
strcpy(ptr, str);\
}

char * getPathPart (char * mnt_fsname)
{
    char	path[DM_MAXPATHLEN];
    char	*p;

    ASSERT (mnt_fsname);

    if (mnt_fsname == NULL) {
	return (NULL);
    }

    for (; (*mnt_fsname != ':') && (*mnt_fsname != '\0'); mnt_fsname++);

    /* ':' is required */
    if (*mnt_fsname == ':') { 

	/* path part may not be 0 */
	if (*(++mnt_fsname) != '\0') { 
	    p= path;
	    for (; *mnt_fsname != '\0'; *p++ = *mnt_fsname++); 
	    *p = '\0';

	    return (cs_hack(path));
	}
    }
    return (NULL);
}


/*
 *  -dmGetDomainHost: returns a char pointer to the static 'domain_host'
 *  which may be the first part of a 'path' (only if path starts with '//').
 *  Example: '//dutenty/users/cacd' its domain_host equals 'dutenty'
 */
static char * _dmGetDomainHost (char * path)
{
    static char	domain_host[NM_SIZE];
    char	*dm, *p;

    ASSERT (path);

    if (path == NULL) {
	return (NULL);
    }

    if ((*path == '/') && (*(path + 1) == '/')) {
	for (p = path + 2, dm = domain_host; (*p != '/') && (*p != '\0'); 
		*dm++ = *p++);
	*dm = '\0';
    }
    else {
	return (NULL);
    }

    return (domain_host);
}


static int existPath (char * path)
{
    struct stat buf;

    ASSERT (path);
    if (path == NULL) {
	return (-1);
    }

    if (stat (path, &buf) < 0) {
	(void) noSignalError (UnixErrorClass, errno, 
		DMSYSERROR, DMNOTFATAL, 0, NULL, NULL);
	(void) noSignalError (RcErrorClass, RCEPATHNOEXIST, 
		DMSYSERROR, DMNOTFATAL, 0, path, NULL);
	return (-1);
    } else {

	/* TO be done: add check on specific dir indic */
	return (0);
    }
}


#if defined( hp9000s300 ) || defined( HPCLUSTER )	/* hp cluster */
static int
_dmTestIfClusterRoot (hostname)
char	*hostname;
{
    /*    Test if hostname == cluster member && cluster root.
     *    If cluster member and no root: generate warning 
     *    (is not a correct project_id)
     */
    struct cct_entry  *cp;

    if ((cp = getccnam (hostname)) != NULL) { 	/* is cluster member */
	if (cp->cnode_type != 'r') {		/* and root */
	    (void) noSignalError (RcErrorClass, RCENOCLUSTERROOT, 
		    DMSYSERROR, DMNOTFATAL, 0, hostname, NULL);
	}

	/* hostname is a cluster member,	*/
	/* so search can be stopped. 		*/
	/* but first close /etc/clusterconf 	*/
	(void) endccent ();
	return (TRUE);
    }

    /* not a cluster member */
    (void) endccent ();
    return (FALSE);
}
#endif /* defined( hp9000s300 ) || defined( HPCLUSTER ) */


/* patrick: temp to get rid of error messages..
 */
static int noSignalError
   (int n1, int n2, int n3, int n4, int n5, char * err, char * tt)
{
/*
fprintf(stderr,"ERROR: (hostnamemaker): %s\n", err);
 */
return n1 + n2 + n3 + n4 + n5 + (int)err + (int)tt;
}
