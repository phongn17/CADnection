#ifndef __S3_STORAGE_H__
#define __S3_STORAGE_H__

//#include <aws/core/Aws.h>
//#include <aws/core/http/HttpRequest.h>
//#include <aws/s3/S3Client.h>
//#include <aws/s3/model/CreateBucketRequest.h>
//#include <aws/s3/model/HeadBucketRequest.h>
//#include <aws/s3/model/PutObjectRequest.h>

//const Aws::S3::Model::BucketLocationConstraint REGION_DEFAULT = Aws::S3::Model::BucketLocationConstraint::us_west_1;

class S3Storage {
public:
	//bool CreateBucketIfAny(const Aws::String &bucketName, const Aws::S3::Model::BucketLocationConstraint &region = REGION_DEFAULT);
	//bool PutObject(const Aws::String& bucketName, const Aws::String& s3ObjectName, const char* filename, const Aws::String& region = "us-west-1");
};

#endif // !__S3_STORAGE_H__
