/*
 * Copyright (c) 2019, Intel Corporation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.

 *
 */


//!
//! \file:   OmafDashSource.h
//! \brief:
//! \detail:
//! Created on May 22, 2019, 3:18 PM
//!

#ifndef OMAFDASHSOURCE_H
#define OMAFDASHSOURCE_H

#include "general.h"
#include "OmafMediaSource.h"
#include "OmafMediaStream.h"
#include "MediaPacket.h"
#include "OmafMPDParser.h"
#include "DownloadManager.h"
#include "OmafExtractorSelector.h"


using namespace VCD::OMAF;

VCD_OMAF_BEGIN

typedef enum{
    STATUS_CREATED=0,
    STATUS_READY,
    STATUS_RUNNING,
    STATUS_EXITING,
    STATUS_STOPPED,
    STATUS_UNKNOWN,
}DASH_STATUS;

typedef std::list<OmafMediaStream*> ListStream;

class OmafDashSource : public OmafMediaSource, Threadable {
public:
    //!
    //! \brief construct
    //!
    OmafDashSource();

    //!
    //! \brief de-construct
    //!
    virtual ~OmafDashSource();

public:
    //!
    //! \brief Interface implementation from base class: OmafMediaSource
    //!
    virtual int OpenMedia(std::string url, std::string cacheDir = "", bool enablePredictor=false);
    virtual int CloseMedia();
    virtual int GetPacket( int streamID, std::list<MediaPacket*>* pkts, bool needParams, bool clearBuf );
    virtual int GetStatistic(DashStatisticInfo* dsInfo);
    virtual int SetupHeadSetInfo(HeadSetInfo* clientInfo);
    virtual int ChangeViewport(HeadPose* pose);
    virtual int GetMediaInfo( DashMediaInfo* media_info );
    virtual int GetTrackCount();
    virtual int SelectSpecialSegments(int extractorTrackIdx);
    //!
    //! \brief Interface implementation from base class: Threadable
    //!
    virtual void Run();

private:
    //!
    //! \brief TimedSelect extractors or adaptation set for streams
    //!
    int TimedSelectSegements( );

    //!
    //! \brief
    //!
    void StopThread();

    //!
    //! \brief update mpd in dynamic mode
    //!
    int TimedUpdateMPD();

    //!
    //! \brief Download Segment in dynamic mode
    //!
    int TimedDownloadSegment( bool bFirst );

    //!
    //! \brief run thread for dynamic mpd processing
    //!
    void thread_dynamic();

    //!
    //! \brief run thread for static mpd processing
    //!
    void thread_static();

    //!
    //! \brief ClearStreams
    //!
    void ClearStreams();

    //!
    //! \brief SeekToSeg
    //!
    void SeekToSeg(int seg_num);

    //!
    //! \brief SetEOS
    //!
    int SetEOS(bool eos);

    //!
    //! \brief Download init Segment
    //!
    int DownloadInitSeg();

    //!
    //! \brief GetSegmentDuration
    //!
    uint64_t GetSegmentDuration(int stream_id);

    //!
    //! \brief Get and Set current status
    //!
    DASH_STATUS GetStatus(){ return mStatus; };
    void SetStatus(DASH_STATUS status){
        pthread_mutex_lock(&mMutex);
        mStatus = status;
        pthread_mutex_unlock(&mMutex);
    };

    //!
    //! \brief Get MPD information
    //!
    MPDInfo*  GetMPDInfo()
    {
        if(!mMPDParser)
            return nullptr;

        return mMPDParser->GetMPDInfo();
    };

    int SyncTime(std::string url);

    int StartReadThread();

private:
    OmafMPDParser*             mMPDParser;                //<! the MPD parser
    DASH_STATUS                mStatus;                   //<! the status of the source
    OmafExtractorSelector*     mSelector;                 //<! the selector for extractor selection
    pthread_mutex_t            mMutex;                    //<! for synchronization
    MPDInfo                    *mMPDinfo;                  //<! MPD information
    int                        dcount;
    GlogWrapper                *m_glogWrapper;
};

VCD_OMAF_END;

#endif /* OMAFSOURCE_H */

